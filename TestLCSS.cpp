
#include <cstring>
#include <iostream>
#include "SSTree.h"
#include "Tools.h"
#include "Windows.h"
using namespace std;


void lcss()
{
    
    const char *A = "aaabbbccc", *B = "ccbbbaa";
    uchar *text = (uchar*)"aaabbbccc$ccbbbaa";
    
    
   
    ulong n = strlen((char*)text);
    n++; 
    
    ulong splitpos = strlen(A); 
    
    cout << "\nCreando suffix tree...\n";
    SSTree *sst = new SSTree(text, n);
    
     
    cout << "... para el texto A$B=";
    cout << (char*)sst->substring(0,n-1) << '\n';   
       

    cout << "\nCalcular la subcadena comun de A y B...\n";

    ulong lastleaf = sst->rightmost(0); 
    ulong i;
    bool *left = new bool[lastleaf+1], *right = new bool[lastleaf+1];
  
        
    for (i=0; i<=lastleaf; i++) {
       left[i] = false; right[i]=false;
    }
        
    for (i=lastleaf;i>0;i--) {
        if (sst->isleaf(i)) {
           if (sst->textpos(i) < splitpos) {left[i]=true;right[i]=false;} 
           else {left[i]=false; right[i]=true;} 
        }
	if (sst->isOpen(i)) {
           left[sst->parent(i)] |= left[i];
           right[sst->parent(i)] |= right[i];             
	}
    }

   
    
   ulong maxdepth=0, maxindex=0;
   for (i=0; i<=lastleaf; i++)
      if (sst->isOpen(i) && left[i]== true && right[i] == true && (sst->depth(i) > maxdepth)) {
         maxdepth = sst->depth(i);
         maxindex = i;
      }

   cout << "lcss(" << A << ',' << B << ")=";
   
   cout << (char*)sst->pathlabel(maxindex) << "\n\n";

   delete sst;
   delete [] left;
   delete [] right;
}

int main()
{
    lcss();
	system("pause");
    return 0;
}

