
#ifndef _SSTREE_H_
#define _SSTREE_H_

#include "SuffixTree.h"
#include "CSA.h"
#include "CHgtArray.h"
#include "BitRank.h"
#include "ReplacePattern.h"
#include "CRMQ.h"
#include "Tools.h"
#include "Parentheses.h"
#include "LcpToParentheses.h"
#include <iostream>

class SSTree : public SuffixTree
{
private:
    ulong n;
    CSA *sa;    
    CHgtArray *hgt;
    CRMQ *rmq;
    ulong *P;
    ReplacePattern *rpLeaf, *rpSibling;
    BitRank *br, *brLeaf, *brSibling;
    Parentheses *Pr;
    
public:
    
    enum io_action
    {
        nop,       
        load_from, 
        save_to    
    };

    SSTree(uchar *, ulong, bool = false, unsigned = 0, io_action = nop, const char * = 0);
    ~SSTree();
    ulong root();
    bool isleaf(ulong) ;
    ulong child(ulong, uchar);
    ulong firstChild(ulong);
    ulong sibling(ulong) ;
    ulong parent(ulong ) ;
    uchar edge(ulong, ulong) ;
    uchar* edge(ulong) ;
    uchar* pathlabel(ulong);
    uchar* substring(ulong, ulong);
    ulong depth(ulong);
    ulong nodeDepth(ulong);
    ulong lca(ulong, ulong);
    ulong lceLinear(uchar *, ulong, ulong);
    ulong lce(ulong, ulong);
    ulong sl(ulong);
    ulong inorder(ulong);
    ulong rightmost(ulong);
    ulong leftmost(ulong);
    ulong leftrank(ulong);
    ulong numberofnodes(ulong);
    ulong numberofleaves(ulong);
    ulong textpos(ulong);
    ulong isOpen(ulong);
    ulong search(uchar *, ulong);
    void PrintHgt();
    ulong lcaParen(ulong, ulong);
    void PrintSA();
    void PrintEdge(ulong);
    void CheckLCA(ulong);
    void PrintTree(ulong, int);
};

#endif
