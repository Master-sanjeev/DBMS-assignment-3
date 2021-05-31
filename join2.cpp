#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include<vector>
#include<fstream>
#include<cmath>
#include <bits/stdc++.h>
//#include "stdc++.h"
#include <cstdlib>

int numOfElementsInPage = PAGE_CONTENT_SIZE / sizeof(int);

using namespace std;

void binarySearchAndAddtoOutput(int s,int binFirstPageNo,int binLastPageNo,FileHandler *inputFileFH,FileHandler *outputFileFH,int* pos,PageHandler* ph,bool* isEmptyOutput);



void binarySearchAndAddtoOutput(int s, int binFirstPageNo, int binLastPageNo, FileHandler* inputFileFH,
                                FileHandler* outputFileFH, int *pos, PageHandler *ph,bool* isEmptyOutput) {

    int num,num1,num2;
    int firstPos = 0;
    int lastPos = numOfElementsInPage - 1;
    bool firstHalf = false;
    bool lastHalf = false;

    //Base Case
    if(binLastPageNo < binFirstPageNo) {
        //printf("\nOut of Binary Search");
        return;
    }
    int binMidPageNo = binFirstPageNo + (binLastPageNo - binFirstPageNo) / 2;


    //printf("\nBinary Code: Search Key = %d , First = %d  Mid = %d  Last= %d  ifh = %u ofh = %u  ", s,
           //binFirstPageNo, binMidPageNo, binLastPageNo, (unsigned int *) inputFileFH, (unsigned int *) outputFileFH);


    //Calling input Page to memory buffer
    //cout << "Getting input page No. : " << binMidPageNo << " \n";

    PageHandler  inputPageHandler = inputFileFH->PageAt(binMidPageNo);
    PageHandler newPageH;
    char *data = inputPageHandler.GetData();
    char *outputData = ph->GetData();


    memcpy(&num1, &data[firstPos * sizeof(int)], sizeof(int));
    memcpy(&num2, &data[lastPos * sizeof(int)], sizeof(int));
    //Only FirstHalf may have search Key
    if (s < num1) {
        firstHalf = true;
    }//Only LastHalf may have search key
    else if (num2!=INT_MIN  and  s > num2) {
        lastHalf = true;
    }//MidPage may have have one/complete search key
    else {
        for (int i = 0; i < numOfElementsInPage; i++) {

            memcpy(&num, &data[i * sizeof(int)], sizeof(int));
            //cout << "Input Data Looked at  : " << num << endl;

            //Match with search key
            if (num == s) {

                if(*isEmptyOutput)
                {
                    *isEmptyOutput = false;
                }

                //If First value,check in firstHalf Pages also
                if (i == 0)
                    firstHalf = true;

                //If Last value,check in lastHalf Pages also
                if (i == numOfElementsInPage - 1)
                    lastHalf = true;

                //cout << "Found Match : Page No: " << binMidPageNo << " Offset :" << i << endl;


                if (*pos == numOfElementsInPage) {
                    *pos = 0;
                    outputFileFH->MarkDirty(ph->GetPageNum());
                    outputFileFH->UnpinPage(ph->GetPageNum());
                    // outputFileFH.FlushPage(ph.GetPageNum());
                    newPageH = outputFileFH->NewPage();
                    *ph = newPageH;
                    outputData = ph->GetData();

                }

                //cout<<"      Filled search Value : " <<s<<" at Page No.: "<<ph->GetPageNum()<<" Pos: " <<(*pos)<<endl;

                num = s;
                memcpy(&outputData[(*pos) * sizeof(int)], &num, sizeof(int));
                (*pos)++;


            }

        }
    }

    //Unpinnng input page after exploring

    inputFileFH->UnpinPage(inputPageHandler.GetPageNum());

    if(firstHalf) {
        int newBinLastPageNo = binMidPageNo - 1;
        binarySearchAndAddtoOutput(s,binFirstPageNo,newBinLastPageNo,inputFileFH,outputFileFH,pos,ph,isEmptyOutput);

    }

    if(lastHalf) {
        int newBinFirstPageNo = binMidPageNo + 1;
        binarySearchAndAddtoOutput(s,newBinFirstPageNo,binLastPageNo,inputFileFH,outputFileFH,pos,ph,isEmptyOutput);

    }


    return;
}

int main(int argc, char *argv[]){

    FileManager fm;

    //cout<<"Opening file "<<argv[1]<<endl;
    FileHandler inputFileFH1 = fm.OpenFile(argv[1]);
    //cout << "Input file opened " << endl;

    //cout<<"Opening file "<<argv[2]<<endl;
    FileHandler inputFileFH2 = fm.OpenFile(argv[2]);
    //cout << "Input file opened " << endl;


    //cout<<"Creating file "<<argv[3]<<endl;
    FileHandler outputFileFH = fm.CreateFile(argv[3]);
    //cout << "Output file created " << endl;



    int* pos = (int*)malloc(sizeof(int));
    int last_value;

    bool* isEmptyOutput = (bool*)malloc(sizeof(bool));
    *isEmptyOutput = true;


    PageHandler *ph = (PageHandler*)malloc(sizeof(PageHandler));
    PageHandler newPageH;
    newPageH = outputFileFH.NewPage();
    *ph = newPageH;



    int lastInput1PageNum = inputFileFH1.LastPage().GetPageNum();

    inputFileFH1.UnpinPage(lastInput1PageNum);

    int lastInput2PageNum = inputFileFH2.LastPage().GetPageNum();

    inputFileFH2.UnpinPage(lastInput2PageNum);

    if(lastInput1PageNum == -1 || lastInput2PageNum == -1){
        
        outputFileFH.DisposePage(ph->GetPageNum());
		outputFileFH.FlushPages();
		fm.CloseFile(inputFileFH1);
		fm.CloseFile(inputFileFH2);
		fm.CloseFile(outputFileFH);
		return 0;
    }


    int firstInput2PageNum = inputFileFH2.FirstPage().GetPageNum();

    inputFileFH2.UnpinPage(firstInput2PageNum);

    int num1;

    char *data1;
    char *outputData;

    //cout<<"last input 1 page num " << lastInput1PageNum << ";; last input 2 page num " << lastInput2PageNum<<endl;

    for (int i = 0; i <= lastInput1PageNum; i++){

        PageHandler pageHandler1 = inputFileFH1.PageAt(i);
        data1 = pageHandler1.GetData();

        for (int k = 0; k < numOfElementsInPage; k++){
            memcpy (&num1, &data1[k*sizeof(int)], sizeof(int));

            binarySearchAndAddtoOutput(num1,firstInput2PageNum,lastInput2PageNum,&inputFileFH2,&outputFileFH,pos,ph,isEmptyOutput);
        }

        inputFileFH1.UnpinPage(i);

    }

    last_value = INT_MIN;

    while(*pos < numOfElementsInPage){
        outputData = ph->GetData();
        memcpy (&outputData[(*pos)*sizeof(int)], &last_value, sizeof(int));
        (*pos)++;
    }


    if(*isEmptyOutput) {
        outputFileFH.DisposePage(ph->GetPageNum());
    }
    else {
        outputFileFH.MarkDirty(ph->GetPageNum());
        outputFileFH.UnpinPage(ph->GetPageNum());
    }

    outputFileFH.FlushPages();

    fm.CloseFile(inputFileFH1);
    fm.CloseFile(inputFileFH2);
    fm.CloseFile(outputFileFH);


}//
// Created by Akshay on 20/04/21.
//

