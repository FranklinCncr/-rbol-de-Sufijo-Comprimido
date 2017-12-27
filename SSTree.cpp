

#include "SSTree.h"

#ifdef SSTREE_HEAPPROFILE
#   include "HeapProfiler.h"
#endif

SSTree::SSTree(uchar *text, ulong n, bool deletetext, unsigned samplerate, io_action IOaction, const char *filename)
{
    #ifdef SSTREE_HEAPPROFILE
        ulong heapCon;
    #endif
    this->n = n;
    unsigned floorLog2n = Tools::FloorLog2(n);
    if (floorLog2n < 4)
        floorLog2n = 4;
    unsigned rmqSampleRate = floorLog2n / 2;
    if (samplerate != 0)
        floorLog2n = samplerate; 
    
    #ifdef SSTREE_TIMER
    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl;
        heapCon = HeapProfiler::GetHeapConsumption();
    #endif

        printf("Creating CSA with samplerate %u\n", floorLog2n);
        fflush(stdout);
        Tools::StartTimer();    
    #endif
    if (IOaction == load_from && filename != 0)
        sa = new CSA(text, n, floorLog2n, (string(filename)+".csa").c_str()); 
    else if (IOaction == save_to && filename != 0)
        sa = new CSA(text, n, floorLog2n, 0, (string(filename)+".csa").c_str()); 
    else // No IO operation
        sa = new CSA(text, n, floorLog2n); 
    #ifdef SSTREE_TIMER
        printf("CSA created in %.0f seconds.\n", Tools::GetTime());

    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl; 
        heapCon = HeapProfiler::GetHeapConsumption();   
    #endif

        printf("Creating CHgtArray\n");
        fflush(stdout);
        Tools::StartTimer();    
    #endif
    if (IOaction == load_from && filename != 0)
        hgt = new CHgtArray(sa, (string(filename)+".lcp").c_str());
    else
        hgt = new CHgtArray(sa, text, n);

    if (IOaction == save_to && filename != 0)
        hgt->SaveToFile((string(filename)+".lcp").c_str());
    #ifdef SSTREE_TIMER
        printf("CHgtArray created in %.0f seconds.\n", Tools::GetTime());
   
    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl;
        heapCon = HeapProfiler::GetHeapConsumption();
    #endif

        printf("Creating parentheses sequence (LcpToParentheses)\n");
        fflush(stdout);
        Tools::StartTimer();
    #endif
    if (deletetext)
       delete [] text;
    
    #ifdef SSTREE_HEAPPROFILE
        heapCon = HeapProfiler::GetHeapConsumption(); 
    #endif
    
    ulong bitsInP;
    if (IOaction == load_from && filename != 0)
        P = LcpToParentheses::GetBalancedParentheses((string(filename)+".bp").c_str(), bitsInP);
    else
        P = LcpToParentheses::GetBalancedParentheses(hgt, n, bitsInP);

    if (IOaction == save_to && filename != 0)
        LcpToParentheses::SaveToFile((string(filename)+".bp").c_str(), P, bitsInP);
    #ifdef SSTREE_TIMER
        printf("Parentheses sequence created in %.0f seconds.\n", Tools::GetTime());
    
    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl;
        heapCon = HeapProfiler::GetHeapConsumption();
    #endif

            
    #endif
    
    #ifdef SSTREE_TIMER
        

        printf("Creating BitRank\n");
        fflush(stdout);
        Tools::StartTimer();    
    #endif
    br = new BitRank(P, bitsInP, false);
    #ifdef SSTREE_TIMER
        printf("BitRank created in %.0f seconds.\n", Tools::GetTime());

    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl;
        heapCon = HeapProfiler::GetHeapConsumption();
    #endif

        printf("Creating Parentheses\n");
        fflush(stdout);
        Tools::StartTimer();    
    #endif
    Pr = new Parentheses(P, bitsInP, true, br);
    #ifdef SSTREE_TIMER
        printf("Parentheses created in %.0f seconds.\n", Tools::GetTime());

    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl;
        heapCon = HeapProfiler::GetHeapConsumption();
    #endif

        printf("Creating ReplacePatterns\n");
        fflush(stdout);
        Tools::StartTimer();    
    #endif
    rpLeaf = new ReplacePattern(1, 8);
    rpSibling = new ReplacePattern(0, 8);
    #ifdef SSTREE_TIMER
        printf("ReplacePatterns created in %.0f seconds.\n", Tools::GetTime());

    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl;
        heapCon = HeapProfiler::GetHeapConsumption();
    #endif

        printf("Creating BitRanks\n");
        fflush(stdout);
        Tools::StartTimer();    
    #endif
    brLeaf = new BitRank(P, bitsInP, false, rpLeaf);       
    brSibling = new BitRank(P, bitsInP, false, rpSibling); 
    
    if (rmqSampleRate < 4)
        rmqSampleRate = 4;
    #ifdef SSTREE_TIMER
        printf("BitRanks created in %.0f seconds.\n", Tools::GetTime());


    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl;
        heapCon = HeapProfiler::GetHeapConsumption();
    #endif

        printf("Creating CRMQ with sample rates %d, %d and %d\n", rmqSampleRate * rmqSampleRate * rmqSampleRate, rmqSampleRate * rmqSampleRate, rmqSampleRate);
        Tools::StartTimer();    
        fflush(stdout);
    #endif
    rmq = new CRMQ(br, P, bitsInP, rmqSampleRate * rmqSampleRate * rmqSampleRate, rmqSampleRate * rmqSampleRate, rmqSampleRate);
    #ifdef SSTREE_TIMER
    #ifdef SSTREE_HEAPPROFILE
        std::cout << "--> HeapProfiler: " << HeapProfiler::GetHeapConsumption() - heapCon << ", " << HeapProfiler::GetMaxHeapConsumption() << std::endl;
    #endif
        printf("CRMQ created in %.0f seconds.\n", Tools::GetTime());
        fflush(stdout);
    #endif
}


SSTree::~SSTree()
{
    delete rmq;
    delete sa;
    delete rpLeaf;
    delete rpSibling;
    delete brLeaf;
    delete brSibling;
    delete br;
    delete hgt;
    delete Pr;
    delete [] P;
}


ulong SSTree::root()
{
    return 0;
}

bool SSTree::isleaf(ulong v)
{
    return !br->IsBitSet(v + 1);
}

ulong SSTree::child(ulong v, uchar c)
{
    if (isleaf(v))
        return 0;
        
    v++;   // First child of v
    while (v != 0)
    {
        if (c == edge(v,1)) 
            return v;
        v = sibling(v);
    }
    return 0;
}

ulong SSTree::firstChild(ulong v)
{
    if (isleaf(v))
        return 0;
        
    return v+1;
}


ulong SSTree::sibling(ulong v)
{
    if (v == 0)
        return 0;

    ulong w = Pr->findclose(parent(v));
    ulong i = Pr->findclose(v) + 1;
    if (i < w)
        return i;

    return 0;   
}


ulong SSTree::parent(ulong v)
{
    return Pr->enclose(v);
}


uchar SSTree::edge(ulong v, ulong d) 
{
    uchar *ss; 
    if (isleaf(v))
    {
        ulong i = leftrank(v);
        ulong j = depth(parent(v));
        ulong d1 = sa->lookup(i) + j;
        if (d > n - d1)
            return 0u;
        ss = sa->substring(d1 + d - 1,1);
        uchar result = ss[0];
        delete [] ss;
        return result;
    }
    
    ulong d1 = hgt->GetPos(inorder(parent(v)));
    ulong d2 = hgt->GetPos(inorder(v));
    if (d > d2 - d1)
        return 0u;
    ss = sa->substring(sa->lookup(inorder(v)) + d1 + d - 1,1);
    uchar result = ss[0];
    delete [] ss;
    return result;    
}


uchar* SSTree::edge(ulong v) 
{
    if (isleaf(v))
    {
        ulong i = leftrank(v);
        ulong j = depth(parent(v));
    ulong k = depth(v);
        ulong d1 = sa->lookup(i) + j;
        return sa->substring(d1,k-j);
    }
    
    ulong d1 = hgt->GetPos(inorder(parent(v)));
    ulong d2 = hgt->GetPos(inorder(v));
   
    return sa->substring(sa->lookup(inorder(v))+d1, d2-d1);    
}

uchar* SSTree::pathlabel(ulong v) 
{
    if (isleaf(v))
    {
        ulong i = leftrank(v);
        ulong k = depth(v);
        ulong d1 = sa->lookup(i);
        return sa->substring(d1,k);
    }
    ulong d2 = hgt->GetPos(inorder(v));
    return sa->substring(sa->lookup(inorder(v)),d2);
}


uchar *SSTree::substring(ulong i, ulong k)
{
   return sa->substring(i,k);
}


ulong SSTree::depth(ulong v)
{
    if (v == 0)
        return 0;

    if (isleaf(v))
    {
        ulong i = leftrank(v);
        return n - sa->lookup(i);
    }
        
    v = inorder(v);
    return hgt->GetPos(v);  
}


ulong SSTree::nodeDepth(ulong v)
{
    return 2 * br->rank(v) - v - 2;
}


ulong SSTree::lca(ulong v, ulong w)
{
    if (v == 0 || w == 0)
        return 0;

    if (v == w || v == root())
        return v;
    if (w == root())
        return root();

    if (v > w)
    {
        ulong temp = w;
        w = v;
        v = temp;
    }
    
    if (Pr->findclose(v) > Pr->findclose(w))
        return v;
    
    return parent(rmq->lookup(v, w) + 1);
}


ulong SSTree::lceLinear(uchar *text, ulong i, ulong j)
{
    ulong k = 0;
    while (text[i+k] == text[j+k])
        k++;
    
    return k;
}


ulong SSTree::lce(ulong i, ulong j)
{
    i = sa->inverse(i);
    ulong v = brLeaf->select(i + 1);
    j = sa->inverse(j);
    ulong w = brLeaf->select(j + 1);
    return depth(lca(v, w));
}


ulong SSTree::sl(ulong v)
{
    if (v == 0 || v == root() || isleaf(v))
        return 0;
    
    ulong x = brLeaf->rank(v - 1) + 1;
    ulong y = brLeaf->rank(Pr->findclose(v));
    x = sa->Psi(x - 1);
    y = sa->Psi(y - 1);
    return lca(brLeaf->select(x + 1), brLeaf->select(y + 1));
}


void SSTree::PrintHgt()
{
    std::cout << "Hgt: [ ";
    for (ulong i = 0; i < n; i++)
        std::cout << i << "=" << hgt->GetPos(i) << " ";
    std::cout << "]\n";
}


void SSTree::PrintSA()
{
    std::cout << "SA: [ ";
    for (ulong i = 0; i < n; i++)
        std::cout << i << "=" << sa->lookup(i) << " ";
    std::cout << "]\n";
}


void SSTree::PrintEdge(ulong v)
{
    ulong k = 1;
    while (edge(v, k) != 0u)
            {
                printf("%c", (int)edge(v, k));
                k++;
            }
           printf("\n");
}


ulong SSTree:: lcaParen(ulong v, ulong w)
{
      
    ulong temp;
    if (v < w) temp = Pr->findclose(w);
    else temp = Pr->findclose(v);
    if (v == w) return w;
    if (v > w)
        v = w;
    
    while (v > 0)
    {
        if (Pr->findclose(v) > temp) return v; 
        v = parent(v); 
    }
    
    return 0;
}


void SSTree::CheckLCA(ulong v)
{
    ulong len = br->rank(rightmost(v));
    v++;ulong w, temp, v1;
    for (w = 1; w < len - 1; w++)
    {
        temp = br->select(w);
        v1 = br->select(w+1);
        while(v1 < len)
        {   
            if (lca(temp, v1) != lcaParen(temp, v1))
            {
                printf("conflict at (%lu, %lu)::lcaParen() = %lu and lca() = %lu\n", temp, v1, lcaParen(temp, v1), lca(temp, v1));
                exit(0);
            }
            v1 = br->select(br->rank(v1)+1);
        }        
        
        if (lca(temp, v1) != lcaParen(temp, v1))
        {
            printf("conflict at (%lu, %lu)::lcaParen() = %lu and lca() = %lu\n", temp, v1, lcaParen(temp, v1), lca(temp, v1));
            exit(0);
        }        
    }
}

void SSTree::PrintTree(ulong v, int depth)
{
    for (int i=0;i< depth;i++)
      printf(" ");
   
    if (v != 0) 
    {
         PrintEdge(v);
    }
    if(!(isleaf(v)))
    {
        PrintTree(v + 1, depth+1);
    }
    v = sibling(v);
    if (v != 0) 
        PrintTree(v, depth);
}


ulong SSTree::rightmost(ulong v)
{
    return brLeaf->select(brLeaf->rank(Pr->findclose(v)));
}

ulong SSTree::leftmost(ulong v)
{
    return brLeaf->select(brLeaf->rank(v)+1);
}

ulong SSTree::leftrank(ulong v)
{
    return brLeaf->rank(v - 1);
}

ulong SSTree::inorder(ulong v)
{
    return brLeaf->rank(Pr->findclose(v+1)) - 1;
}

ulong SSTree::numberofnodes(ulong v)
{
    return (Pr->findclose(v)-v-1)/2+1;
}

ulong SSTree::numberofleaves(ulong v)
{
    return leftrank(Pr->findclose(v))-leftrank(v);
}

ulong SSTree::textpos(ulong v)
{
     
    return sa->lookup(this->leftrank(v));
}


ulong SSTree::isOpen(ulong v)
{
    return Pr->isOpen(v);
}


ulong SSTree::search(uchar *pattern, ulong l)
{
    if (l == 0)
        return 0;
        
    ulong sp, ep;
    if (!sa->Search(pattern, l, &sp, &ep))
        return root();       
    
    
    sp = brLeaf->select(sp+1);
    ep = brLeaf->select(ep+1);

    
    if (sp == ep)
        return sp;
    ep ++;

    
    ulong r_sp = br->rank(sp),
          r_ep = br->rank(ep);
    
    ulong open = r_ep - r_sp + 1,
          close = (ep - r_ep) - (sp - r_sp);
    
    
    if (open < close) 
    {
        sp   -= close - open;
        r_sp -= close - open;  
    }
    if (open > close)
        ep += open - close;    
    
    
    close = sp - r_sp + 1;
    
    if (close > 0)
        close = br->select0(close);
    else
        close = 0;  

    
    open = br->rank(br->NumberOfBits() - 2);
    
    if (r_ep + 1 <= open)
        open = br->select(r_ep + 1);
    else
        open = br->NumberOfBits() - 1; 
    
    
    if (sp - close <= open - ep)
        return close + 1;
    else
        return sp - (open - ep) + 1;
}

