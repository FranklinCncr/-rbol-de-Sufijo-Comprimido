
#include <cstring>
#include <iostream>
#include "SSTree.h"
#include "Tools.h"
#include "Windows.h"
#include <fstream>
#include <ctime> 
#include "psapi.h"

using namespace std;

void lcss()
{
		
	string str;
	unsigned t0, t1,t2,t3;

	fstream archive("adn/adn15.txt");

	archive >> str;
	//const char *A = "aaabbbcccc", *B = "ccccbbbaa";
	uchar *text = (uchar*)(const unsigned char *)str.c_str();
	//uchar *text = (uchar*)"aaabbbcccccccbbbaa";
	
    ulong n = strlen((char*)text);
    n++; 
    
    ulong splitpos = 1; 
    
    cout << "\nCreando suffix tree...\n";
	t0 = clock();
    SSTree *sst = new SSTree(text, n);
	t1 = clock();
	double time = (double(t1 - t0) / CLOCKS_PER_SEC);
    
	//PRINT SCREEN
    cout << "... para el texto ADN=";
    cout << (char*)sst->substring(0,n-1) << '\n';   
       

	cout << "Tiempo de Construccion =" <<time<< endl;

	cout << "\nCalculando la subcadena comun...\n";
	t2 = clock();

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

   cout << "lcss=";
   
   cout << (char*)sst->pathlabel(maxindex) << "\n\n";
   t3 = clock();
   double time2 = (double(t3 - t2) / CLOCKS_PER_SEC);
   cout << "Tiempo de consulta ="<<time2 << endl;
   

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

