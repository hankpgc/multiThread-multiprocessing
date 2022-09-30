// 
// OS_HW1
// ��T�T�A 10827234 �^�ۺ�
//

#include <thread>
#include <mutex> // �h���� �W��!! 
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

mutex g_mutex; // �h������I�s�P�@�Ө禡(�� mutex ��)��������e�ήɡA�䥦������n�s���Ӹ귽�ɡA�N�|�Q�צ�A
string inFileName = "";
float startT = 0.0, endT = 0.0; 				// ����ɶ�

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
    vector<vector<int>> allSorted; // �s�U�Ӥ��Ϊ�vector
    int temp; // Ū�ɮɯ��s�Ʀr
    int k = 0, eachSize = 0 ; // �N��Ƥ���K��
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
            cout << "�ݢ���������������������������������������" << endl;
            cout << "��     Mission 1     ��" << endl;
            cout << "�㢤��������������������������������������" << endl;

            if ( openFile(inputFile) ) { // read records from a file
                
                while (inputFile >> temp) {
                    inputNums.push_back(temp);
                } // while

                inputFile.close();

                startT = clock();
                NewBubbleSort(inputNums);
                endT = clock();

                writeFile(inputNums, "1"); // ��k�@
                
                cout << "input length :" << inputNums.size() << endl;
                cout << "CPU Time :" << (float) (endT - startT) / 1000 << "s"  << endl;
                cout << "datetime :" << currentDateTime() << endl;
            } // if

            break;

        case 2 :
            cout << "�ݢ���������������������������������������" << endl;
            cout << "��     Mission 2     ��" << endl;
            cout << "�㢤��������������������������������������" << endl;

            if ( openFile(inputFile) ) { // read records from a file
                while (inputFile >> temp) {
                    inputNums.push_back(temp);
                } // while
                
                inputFile.close();

                k = inputK(inputNums.size()) ;
                eachSize = inputNums.size() / k; //�C�@����

                startT = clock();
                for( int x = 0 ; x < k; x++) {
                    vector<int> tempV;
                    
                    if( x == k-1 ) // �̫�@��
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.end()); // �ѤU���㰣��������i�̫�@�Ӱ��B�z
                    else 
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.begin() + (x+1) * eachSize );
                    
                    NewBubbleSort(tempV);
                    allSorted.push_back(tempV);
                } // for

                vResult = k_way_MergeSort(allSorted, inputNums.size());
                endT = clock();

                writeFile(vResult, "2"); // ��k�G    

                cout << "\noutput length :" << vResult.size() << endl;
                cout << "CPU Time :" << (float) (endT - startT) / 1000 << "s"  << endl;
                cout << "datetime :" << currentDateTime() << endl;
            } // if
            break;

        case 3:
            cout << "�ݢ���������������������������������������" << endl;
            cout << "��     Mission 3     ��" << endl;
            cout << "�㢤��������������������������������������" << endl;

            if ( openFile(inputFile) ) { // read records from a file
                while (inputFile >> temp) {
                    inputNums.push_back(temp);
                } // while
                
                inputFile.close();

                k = inputK(inputNums.size()) ;
                eachSize = inputNums.size() / k; //�C�@����

                startT = clock(); 
                for( int x = 0 ; x < k; x++) {// �N�F����� n ���ɮ�
                    vector<int> tempV;
                    
                    if( x == k-1 ) // �̫�@��
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.end()); // �ѤU���㰣��������i�̫�@�Ӱ��B�z
                    else
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.begin() + (x+1) * eachSize );

                    bubbleOutFile(tempV, to_string(x));
                } // for

                vector<STARTUPINFO> siALL;
                vector<PROCESS_INFORMATION> piALL;
                HANDLE * handles = new HANDLE[k] ; // �� k �� process
                HANDLE * threads = new HANDLE[k] ; // �����D���ƻ�n�[�o�Ӥ~��R�������ɮ�
                for( i = 0 ; i < k ; i++ ) {
                    STARTUPINFO si;  
                    PROCESS_INFORMATION pi;  
                    ZeroMemory(&si, sizeof(si));  
                    ZeroMemory(&pi, sizeof(pi)); 

                    siALL.push_back(si) ;
                    piALL.push_back(pi) ;

                    // �H�U���ǤJ���Ѽ�
                    char tempParm[10] = "" ; // �Ĥ@�ӰѼƪ�� �n�B�z�ĴX���ɮ�
                    strcpy(tempParm, to_string(i).c_str()); 
                    LPTSTR  cmdLine = (LPTSTR) tempParm;// ���O�ഫ

                    string temp = "bubblesort.exe";  // �n���檺�i������(�m��P�ؿ��U)
                    LPCTSTR  exeName = (LPCTSTR)temp.c_str() ;// ���O�ഫ

                    //�إߤ@�ӷs�{��
                    if(CreateProcess(  
                        exeName,   //  ���V�@��NULL�������B�Ψӫ��w�i����Ҳժ��e�줸�զr��
                        cmdLine, // �R�O�C�r��
                        NULL, //    ���V�@��SECURITY_ATTRIBUTES���c��A�o�ӵ��c��M�w�O�_��^������N�X�i�H�Q�l�{���~�ӡC
                        NULL, //    �p�GlpProcessAttributes�޼Ƭ��š]NULL�^�A���򱱨�N�X����Q�~�ӡC<�P�W>
                        false,//    ���ܷs�{�ǬO�_�q�I�s�{�ǳB�~�ӤF����N�X�C 
                        0,  //  ���w���[���B�Ψӱ����u�����M�{�Ǫ��إߪ���
                            //  CREATE_NEW_CONSOLE  �s�����i�}�Ҥl�{��
                            //  CREATE_SUSPENDED    �l�{�ǫإ߫᱾�_�A����I�sResumeThread�禡
                        NULL, //    ���V�@�ӷs�{�Ǫ����Ҷ��C�p�G���޼Ƭ��šA�s�{�ǨϥΩI�s�{�Ǫ�����
                        NULL, //    ���w�l�{�Ǫ��u�@���|
                        &siALL.at(i), // �M�w�s�{�Ǫ��D����p����ܪ�STARTUPINFO���c��
                        &piALL.at(i)  // �����s�{�Ǫ��ѧO��T��PROCESS_INFORMATION���c��
                        ))  {  
                        
                        // �N PROCESS_INFORMATION ���� hProcess ��W�x�s
                        // hProcess ���O�� HANDLE
                        handles[i]= piALL.at(i).hProcess;
                        threads[i]= piALL.at(i).hThread;
                    }  // if
                    else{  
                        cerr << "failed to create process" << endl;  
                    }  // else

                } // for

                WaitForMultipleObjectsExpand( handles, (DWORD)k); // multiprocess
                // ���ݩҦ��lprocess ����
                
                Sleep(200);  // 

                // �Ҧ��ɮצs���W��vector
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
                    remove(fileName.c_str()) ;// �R���ɮ�

                    CloseHandle( handles[i] ); // hProcess
                    CloseHandle( threads[i] );
                } // for
                
                vResult = k_way_MergeSort(allSorted, inputNums.size()) ;
                endT = clock();
                //MergeSort(sortedBubble, 0, sortedBubble.size() - 1) ;
                writeFile(vResult, "3"); // ��k�T
                
                cout << "\noutput length :" << vResult.size() << endl;
                cout << "CPU Time :" << (float) (endT - startT) / 1000 << "s"  << endl;
                cout << "datetime :" << currentDateTime() << endl;
            } // if
            
            break;

        case 4:
            cout << "�ݢ���������������������������������������" << endl;
            cout << "��     Mission 4     ��" << endl;
            cout << "�㢤��������������������������������������" << endl;

            if ( openFile(inputFile) ) { // read records from a file
                while (inputFile >> temp) {
                    inputNums.push_back(temp);
                } // while
                
                inputFile.close();

                k = inputK(inputNums.size()) ;
                eachSize = inputNums.size() / k; //�C�@����

                startT = clock();
                for( int x = 0 ; x < k; x++) {
                    vector<int> tempV;
                    
                    if( x == k-1 ) // �̫�@��
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.end()); // �ѤU���㰣��������i�̫�@�Ӱ��B�z
                    else 
                        tempV.assign(inputNums.begin() + (x * eachSize), inputNums.begin() + (x+1) * eachSize );

                    allSorted.push_back(tempV);
                    // �� thread ���� bubble sort  function �å� ref() ��� �F�� call by referance
                    //threads.push_back(thread(threadNewBubbleSort, std::ref(allSorted.at(x))));// �гy k �� thread ��i vector ��
                } // for

                // �q�W�����g�k�ԤU�ӿW�߰j��N�i�H�F???????????
                for ( i = 0; i < k; i++) {
                    threads.push_back(thread(threadNewBubbleSort, std::ref(allSorted.at(i))));// �гy k �� thread ��i vector ��
                } // for

                for (int x = 0; x < threads.size(); x++) { // ���� thread ����
                    threads.at(x).join();
                }// for

                vResult = k_way_MergeSort(allSorted, inputNums.size()) ; 
                endT = clock();
                writeFile(vResult, "4"); // ��k�| 

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
        cout << "�п�J�n�����X��: "  ; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!�������Ƥ���j���Ƥj�p!!!!!!!!!!!!!!!!!!!!
        cin >> input;
        char a[input.length()];
        strcpy(a,input.c_str());
	 
        for( int i = 0 ; i < input.length() ; i++){
            //�p�G���O?�r
            if(!isdigit(a[i])){
                cout << endl <<  "## ��J���~�п�J�X�z������� ##" << endl ;
                correct = false ; 
            } // if
        } // for

        if( correct ) {
            k = atoi(a); // �N�r���ন���
            
            if( k <= 0 || k > dataSize ) {
                cout << endl <<  "## ��J���~�п�J�X�z������� && ���i�j���Ƥj�p ##" << endl ;
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

	return false ; // return �᭫�s��Jcommand 
} // openInputFile() 

void writeFile( vector<int> vTemp, string taskNum ) { // write datas into a file
	string fileName = inFileName + "_output" + taskNum + ".txt" ;
	fstream temp ; // �ɮת��� 
	
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
	fstream temp ; // �ɮת��� 
	
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
    // g_mutex �a�J lock_guard �غc�ɤW��A�������} print �禡�ɡAlock_guard �ͩR�g���]�H�������Alock_guard �i��Ѻc�ɹ� g_mutex ����

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

vector<int> k_way_MergeSort(vector<vector<int>> vTemp, int dataSize) { 	// head �P last ���x�}�d��
    vector<int> result;
    int minimize = 0;
    int whichVector = 0;
    for( int i = 0 ; i < dataSize ; i ++){

        minimize = vTemp.at(0).at(0) ; // �� 1 ��vector ���Ĥ@�Ӥ���
        whichVector = 0 ;
        for( int x = 1 ; x < vTemp.size() ; x++) { // �̧ǳX�ݩҦ� vector<int> ���Ĥ@�ӭ� ��̤p
            if( vTemp.at(x).at(0) < minimize ) { // �� x ��vector ���Ĥ@�Ӥ��� �� minimize �٤p ( �ثe�̤p)
                minimize = vTemp.at(x).at(0) ; 
                whichVector = x ;
            } // if
        } // inner for()

        result.push_back(minimize) ;
        vTemp.at(whichVector).erase(vTemp.at(whichVector).begin()); // ���̤p���Ȧb�Y�}�C
        // �N�Ȩ��X��쵲�G vector �çR��

        if( vTemp.at(whichVector).size() == 0 ) // �Y��vector ���� �R��
            vTemp.erase(vTemp.begin() + whichVector) ;

    } // out for()
    
    return result;
} // MergeSort()

void WaitForMultipleObjectsExpand(HANDLE * handles, DWORD count) {
    DWORD index = 0;
    DWORD result = 0;
    DWORD handleCount = count;

    /// �C64��HANDLE��?�@�� (MAXIMUM_WAIT_OBJECTS)
    while (handleCount >= MAXIMUM_WAIT_OBJECTS) {
        WaitForMultipleObjects(MAXIMUM_WAIT_OBJECTS, &handles[index], TRUE, INFINITE);

        handleCount -= MAXIMUM_WAIT_OBJECTS;
        index += MAXIMUM_WAIT_OBJECTS;
    } // while

    if (handleCount > 0) {
        WaitForMultipleObjects(handleCount, &handles[index], TRUE, INFINITE);
    } // if
} // WaitForMultipleObjectsExpand