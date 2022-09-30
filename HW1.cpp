// 
// OS_HW1
// 資訊三乙 10827234 彭桂綺
//

#include <thread>
#include <mutex> // 多執序 上鎖!! 
#include <windows.h>
#include <iostream>
#include <string> // getline
#include <string.h> // strtok_s
#include <vector>
#include <fstream>	// open, is_open, close, write, ignore
#include <time.h>
#include <ctime>
#include <stdlib.h> 
#include <algorithm>
using namespace std;

mutex g_mutex; // 多執行緒呼叫同一個函式(有 mutex 鎖)當有執行緒占用時，其它執行緒要存取該資源時，就會被擋住，
string inFileName = "";
float startT = 0.0, endT = 0.0; 				// 執行時間

int inputK( int dataSize );
bool openFile( fstream &inFile ) ; // read records from a file
void writeFile( vector<int> vTemp, string taskNum ) ; // write datas  into a file
void bubbleOutFile( vector<int> vTemp, string taskNum ) ;

void NewBubbleSort(vector<int>& vTemp);
void threadNewBubbleSort(vector<int>& vTemp) ;
vector<int> k_SliceBubbleSort(vector<int> inputNums, int k, int eachSize) ;
const string currentDateTime();

vector<int> k_way_MergeSort( vector<vector<int>> vTemp, int dataSize ) ;
void WaitForMultipleObjectsExpand(HANDLE * handles, DWORD count);

int main() {
    string command = "0";
    vector<int> inputNums;
    vector<thread> threads;
    vector<int> sortedBubble;
    vector<int> vResult;
    vector<vector<int>> allSorted; // 存各個分割的vector
    int temp; // 讀檔時站存數字
    int k = 0, eachSize = 0 ; // 將資料切成K份
    int i = 0, j = 0 ;

    do {
        cout << endl << "*********************  MultiThreading  **********************";
        cout << endl << "* 1. Bubble sort                                            *";
        cout << endl << "* 2. k-data & one Process Bubble sort + Merge sort          *";
        cout << endl << "* 3. k-data & k-process Bubble sort + n-process Merge sort  *";
        cout << endl << "* 4. k-data & k-thread Bubble sort + n-thread Merge sort    *";
        cout << endl << "*************************************************************" << endl;
        cout << "Please input a choice(1, 2, 3, 4, [0]:quit!):";
        cin >> command;

        fstream inputFile;
        /* Initialize */ inputNums.clear(); threads.clear(); sortedBubble.clear() ;
        allSorted.clear() ;  vResult.clear() ;

        switch (atoi(command.c_str())) {
        case 1 :
            cout << "╔═══════════════════╗" << endl;
            cout << "║     Mission 1     ║" << endl;
            cout << "╚═══════════════════╝" << endl;

            if ( openFile(inputFile) ) { // read records from a file
                
                while (inputFile >> temp) {
                    inputNums.push_back(temp);
                } // while

                inputFile.close();

                startT = clock();
                NewBubbleSort(inputNums);
                endT = clock();

                writeFile(inputNums, "1"); // 方法一
                
                cout << "input length :" << inputNums.size() << endl;
                cout << "CPU Time :" << (float) (endT - startT) / 1000 << "s"  << endl;
                cout << "datetime :" << currentDateTime() << endl;
            } // if

            break;

        case 2 :
            cout << "╔═══════════════════╗" << endl;
            cout << "║     Mission 2     ║" << endl;
            cout << "╚═══════════════════╝" << endl;

            if ( openFile(inputFile) ) { // read records from a file
                while (inputFile >> temp) {
                    inputNums.push_back(temp);
                } // while
                
                inputFile.close();

                k = inputK(inputNums.size()) ;
                eachSize = inputNums.size() / k; //每一份有

                startT = clock();
                for( int x = 0 ; x < k; x++) {
                    vector<int> tempV;
                    
                    if( x == k-1 ) // 最後一份
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.end()); // 剩下不整除元素都放進最後一個做處理
                    else 
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.begin() + (x+1) * eachSize );
                    
                    NewBubbleSort(tempV);
                    allSorted.push_back(tempV);
                } // for

                vResult = k_way_MergeSort(allSorted, inputNums.size());
                endT = clock();

                writeFile(vResult, "2"); // 方法二    

                cout << "\noutput length :" << vResult.size() << endl;
                cout << "CPU Time :" << (float) (endT - startT) / 1000 << "s"  << endl;
                cout << "datetime :" << currentDateTime() << endl;
            } // if
            break;

        case 3:
            cout << "╔═══════════════════╗" << endl;
            cout << "║     Mission 3     ║" << endl;
            cout << "╚═══════════════════╝" << endl;

            if ( openFile(inputFile) ) { // read records from a file
                while (inputFile >> temp) {
                    inputNums.push_back(temp);
                } // while
                
                inputFile.close();

                k = inputK(inputNums.size()) ;
                eachSize = inputNums.size() / k; //每一份有

                startT = clock(); 
                for( int x = 0 ; x < k; x++) {// 將東西分成 n 個檔案
                    vector<int> tempV;
                    
                    if( x == k-1 ) // 最後一份
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.end()); // 剩下不整除元素都放進最後一個做處理
                    else
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.begin() + (x+1) * eachSize );

                    bubbleOutFile(tempV, to_string(x));
                } // for

                vector<STARTUPINFO> siALL;
                vector<PROCESS_INFORMATION> piALL;
                HANDLE * handles = new HANDLE[k] ; // 有 k 個 process
                HANDLE * threads = new HANDLE[k] ; // 不知道為甚麼要加這個才能刪除全部檔案
                for( i = 0 ; i < k ; i++ ) {
                    STARTUPINFO si;  
                    PROCESS_INFORMATION pi;  
                    ZeroMemory(&si, sizeof(si));  
                    ZeroMemory(&pi, sizeof(pi)); 

                    siALL.push_back(si) ;
                    piALL.push_back(pi) ;

                    // 以下為傳入的參數
                    char tempParm[10] = "" ; // 第一個參數表示 要處理第幾個檔案
                    strcpy(tempParm, to_string(i).c_str()); 
                    LPTSTR  cmdLine = (LPTSTR) tempParm;// 型別轉換

                    string temp = "bubblesort.exe";  // 要執行的可執行檔(置於同目錄下)
                    LPCTSTR  exeName = (LPCTSTR)temp.c_str() ;// 型別轉換

                    //建立一個新程序
                    if(CreateProcess(  
                        exeName,   //  指向一個NULL結尾的、用來指定可執行模組的寬位元組字串
                        cmdLine, // 命令列字串
                        NULL, //    指向一個SECURITY_ATTRIBUTES結構體，這個結構體決定是否返回的控制代碼可以被子程序繼承。
                        NULL, //    如果lpProcessAttributes引數為空（NULL），那麼控制代碼不能被繼承。<同上>
                        false,//    指示新程序是否從呼叫程序處繼承了控制代碼。 
                        0,  //  指定附加的、用來控制優先類和程序的建立的標
                            //  CREATE_NEW_CONSOLE  新控制檯開啟子程序
                            //  CREATE_SUSPENDED    子程序建立後掛起，直到呼叫ResumeThread函式
                        NULL, //    指向一個新程序的環境塊。如果此引數為空，新程序使用呼叫程序的環境
                        NULL, //    指定子程序的工作路徑
                        &siALL.at(i), // 決定新程序的主窗體如何顯示的STARTUPINFO結構體
                        &piALL.at(i)  // 接收新程序的識別資訊的PROCESS_INFORMATION結構體
                        ))  {  
                        
                        // 將 PROCESS_INFORMATION 中的 hProcess 單獨儲存
                        // hProcess 型別為 HANDLE
                        handles[i]= piALL.at(i).hProcess;
                        threads[i]= piALL.at(i).hThread;
                    }  // if
                    else{  
                        cerr << "failed to create process" << endl;  
                    }  // else

                } // for

                WaitForMultipleObjectsExpand( handles, (DWORD)k); // multiprocess
                // 等待所有子process 結束
                
                Sleep(200);  // 

                // 所有檔案存成獨立vector
                for(i = 0 ; i < k ; i++) {
                    string fileName = "bubble_";
                    fileName +=  to_string(i);
                    fileName += ".txt" ;
                    inputFile.open(fileName, ios::in);

                    vector<int> vTemp;
                    if( inputFile ) {
                        while (inputFile >> temp) {
                            vTemp.push_back(temp);
                        } // while

                        allSorted.push_back(vTemp) ;
                    } // if

                    inputFile.close() ;
                    remove(fileName.c_str()) ;// 刪掉檔案

                    CloseHandle( handles[i] ); // hProcess
                    CloseHandle( threads[i] );
                } // for
                
                vResult = k_way_MergeSort(allSorted, inputNums.size()) ;
                endT = clock();
                //MergeSort(sortedBubble, 0, sortedBubble.size() - 1) ;
                writeFile(vResult, "3"); // 方法三
                
                cout << "\noutput length :" << vResult.size() << endl;
                cout << "CPU Time :" << (float) (endT - startT) / 1000 << "s"  << endl;
                cout << "datetime :" << currentDateTime() << endl;
            } // if
            
            break;

        case 4:
            cout << "╔═══════════════════╗" << endl;
            cout << "║     Mission 4     ║" << endl;
            cout << "╚═══════════════════╝" << endl;

            if ( openFile(inputFile) ) { // read records from a file
                while (inputFile >> temp) {
                    inputNums.push_back(temp);
                } // while
                
                inputFile.close();

                k = inputK(inputNums.size()) ;
                eachSize = inputNums.size() / k; //每一份有

                startT = clock();
                for( int x = 0 ; x < k; x++) {
                    vector<int> tempV;
                    
                    if( x == k-1 ) // 最後一份
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.end()); // 剩下不整除元素都放進最後一個做處理
                    else 
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.begin() + (x+1) * eachSize );

                    allSorted.push_back(tempV);
                    // 用 thread 執行 bubble sort  function 並用 ref() 函數 達成 call by referance
                    //threads.push_back(thread(threadNewBubbleSort, std::ref(allSorted.at(x))));// 創造 k 個 thread 丟進 vector 中
                } // for

                // 從上面的寫法拉下來獨立迴圈就可以了???????????
                for ( i = 0; i < k; i++) {
                    threads.push_back(thread(threadNewBubbleSort, std::ref(allSorted.at(i))));// 創造 k 個 thread 丟進 vector 中
                } // for

                for (int x = 0; x < threads.size(); x++) { // 等待 thread 結束
                    threads.at(x).join();
                }// for

                vResult = k_way_MergeSort(allSorted, inputNums.size()) ; 
                endT = clock();
                writeFile(vResult, "4"); // 方法四 

                cout << "\noutput length :" << vResult.size() << endl;
                cout << "CPU Time :" << (float) (endT - startT) / 1000 << "s"  << endl;
                cout << "datetime :" << currentDateTime() << endl;
            } // if
            
            break;

        case 0:break;
        default: cout << endl << "Command does not exist!" << endl;
        } // switch(command)
        
    } while (command != "0");

    system("pause");
    return 0;
} // main()

int inputK( int dataSize ){
    int k = 0;
    bool correct = true ;
    string input;

    do {
        correct = true ;
        cout << "請輸入要切成幾份: "  ; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!切的份數不能大於資料大小!!!!!!!!!!!!!!!!!!!!
        cin >> input;
        char a[input.length()];
        strcpy(a,input.c_str());
	 
        for( int i = 0 ; i < input.length() ; i++){
            //如果不是?字
            if(!isdigit(a[i])){
                cout << endl <<  "## 輸入錯誤請輸入合理的正整數 ##" << endl ;
                correct = false ; 
            } // if
        } // for

        if( correct ) {
            k = atoi(a); // 將字串轉成整數
            
            if( k <= 0 || k > dataSize ) {
                cout << endl <<  "## 輸入錯誤請輸入合理的正整數 && 不可大於資料大小 ##" << endl ;
                correct = false ;
            } // if
        } // if

    }while( !correct );
    
    return k ;
} // inputK

bool openFile( fstream &inFile ) { // read records from a file
	string fileName = "";

    do {
        cout << endl << "Input a file number(eg.input_10w) ([0]:quit!):";
        cin >> inFileName;

        if(strcmp(inFileName.c_str(), "0") == 0 )
            break ;

        fileName = inFileName + ".txt";
        inFile.open(fileName, ios::in); 
        if( inFile )
            return true ;
        else 
            cout << endl << "### " << fileName << " does not exist! ###" << endl ;

    }while( !inFile ) ;

	return false ; // return 後重新輸入command 
} // openInputFile() 

void writeFile( vector<int> vTemp, string taskNum ) { // write datas into a file
	string fileName = inFileName + "_output" + taskNum + ".txt" ;
	fstream temp ; // 檔案物件 
	
	temp.open( fileName, ios :: out ) ; 

    temp << "Sort :" << endl ;
	for( unsigned int i = 0 ; i < vTemp.size() ; i++ ) 
	   	temp << vTemp[i] << endl ;

    temp << "CPU Time :" << (float) (endT - startT) / 1000 << "s"  << endl;
    temp << "datetime :" << currentDateTime() << endl;

    temp.close() ;

	return ;
} // WriteFile()

void bubbleOutFile( vector<int> vTemp, string taskNum ) { // write datas  into a file
	string fileName = "bubble_" + taskNum + ".txt" ;
	fstream temp ; // 檔案物件 
	
	temp.open( fileName, ios :: out ) ; 

	for( unsigned int i = 0 ; i < vTemp.size() ; i++ ) 
	   	temp << vTemp[i] << endl ;

	return ;
} // WriteFile()

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

void threadNewBubbleSort(vector<int>& vTemp) {
    
    lock_guard<mutex> lock(g_mutex); // crtical section
    // g_mutex 帶入 lock_guard 建構時上鎖，之後離開 print 函式時，lock_guard 生命週期也隨之結束，lock_guard 進行解構時對 g_mutex 解鎖

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
   
} // BubbleSort()

const string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    localtime_s(&tstruct, &now);

    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

vector<int> k_way_MergeSort(vector<vector<int>> vTemp, int dataSize) { 	// head 與 last 為矩陣範圍
    vector<int> result;
    int minimize = 0;
    int whichVector = 0;
    for( int i = 0 ; i < dataSize ; i ++){

        minimize = vTemp.at(0).at(0) ; // 第 1 個vector 的第一個元素
        whichVector = 0 ;
        for( int x = 1 ; x < vTemp.size() ; x++) { // 依序訪問所有 vector<int> 的第一個值 找最小
            if( vTemp.at(x).at(0) < minimize ) { // 第 x 個vector 的第一個元素 比 minimize 還小 ( 目前最小)
                minimize = vTemp.at(x).at(0) ; 
                whichVector = x ;
            } // if
        } // inner for()

        result.push_back(minimize) ;
        vTemp.at(whichVector).erase(vTemp.at(whichVector).begin()); // 找到最小的值在某陣列
        // 將值取出放到結果 vector 並刪除

        if( vTemp.at(whichVector).size() == 0 ) // 某個vector 為空 刪除
            vTemp.erase(vTemp.begin() + whichVector) ;

    } // out for()
    
    return result;
} // MergeSort()

void WaitForMultipleObjectsExpand(HANDLE * handles, DWORD count) {
    DWORD index = 0;
    DWORD result = 0;
    DWORD handleCount = count;

    /// 每64個HANDLE分?一組 (MAXIMUM_WAIT_OBJECTS)
    while (handleCount >= MAXIMUM_WAIT_OBJECTS) {
        WaitForMultipleObjects(MAXIMUM_WAIT_OBJECTS, &handles[index], TRUE, INFINITE);

        handleCount -= MAXIMUM_WAIT_OBJECTS;
        index += MAXIMUM_WAIT_OBJECTS;
    } // while

    if (handleCount > 0) {
        WaitForMultipleObjects(handleCount, &handles[index], TRUE, INFINITE);
    } // if
} // WaitForMultipleObjectsExpand