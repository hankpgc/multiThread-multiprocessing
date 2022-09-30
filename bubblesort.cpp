#include <vector>
#include <stdlib.h> 
#include <string> // getline
#include <string.h> // strtok_s
#include <iostream>
#include <cstring>
#include <tchar.h>
#include <iostream>
#include <fstream>
using namespace std;


void NewBubbleSort(vector<int>& vTemp);

int main( int argc, char ** argv ){

    fstream inputFile, outFile;
    int tempNum = 0 ;
    vector<int> sortedBubble;

    string fileName = "bubble_";
    fileName += argv[0]; // argv[0] 傳入的參數為 第幾個檔案
    fileName += ".txt" ;
    inputFile.open(fileName, ios::in );

    if( inputFile ) {
        while (inputFile >> tempNum) {
            sortedBubble.push_back(tempNum);
        } // while
    } // if

    inputFile.close() ;

    NewBubbleSort(sortedBubble) ;

    outFile.open(fileName, ios::out | ios::trunc); // 開啟檔案且清除原來數據
    for( unsigned int i = 0 ; i < sortedBubble.size() ; i++ ) 
	   	outFile << sortedBubble[i] << endl ; // 將排序好的寫回檔案
    
    return 0;
} // main()


void NewBubbleSort(vector<int>& vTemp) {
    bool sorted = true;

    unsigned int n = vTemp.size() - 1;

    for (unsigned int i = 0; i <= n && sorted; i++) {
        sorted = false;
        for( unsigned int j = 1 ; j <= n ; j++) {
            if (vTemp[j - 1] > vTemp[j]) {
                swap(vTemp[j], vTemp[j - 1]);
                sorted = true;
            } // end if()
        } // inner for
    } // out for
   
    return;
} // BubbleSort()