#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include<vector>
#include<fstream>
#include<cmath>
//#include <bits/stdc++.h>
#include <bits/stdc++.h>
#include <cstdlib>


 int numOfElementsInPage = PAGE_CONTENT_SIZE / sizeof(int);


void binarySearchAndAddtoOutput(int s,int binFirstPageNo,int binLastPageNo,FileHandler *inputFileFH,FileHandler *outputFileFH,int *totalInBuffer,int* pos,PageHandler* ph);

using namespace std;

void binarySearchAndAddtoOutput(int s, int binFirstPageNo, int binLastPageNo, FileHandler* inputFileFH,
                                FileHandler* outputFileFH, int *totalInBuffer, int *pos, PageHandler *ph) {

    int num, num1, num2;
    int firstPos = 1;
    int lastPos = numOfElementsInPage - 1;
    bool firstHalf = false;
    bool lastHalf = false;

    //Base Case
    if(binLastPageNo < binFirstPageNo) {
        //printf("\nOut of Binary Search");
        return;
    }
    int binMidPageNo = binFirstPageNo + (binLastPageNo - binFirstPageNo) / 2;


    //printf("\nBinary Code: Search Key = %d , First = %d  Mid = %d  Last= %d  ifh = %u ofh = %u  MemoryBuffer Size = %d ", s,
           //binFirstPageNo, binMidPageNo, binLastPageNo, (unsigned int *) inputFileFH, (unsigned int *) outputFileFH, *(totalInBuffer));


    //Calling input Page to memory buffer
    //cout << "Getting input page No. : " << binMidPageNo << " \n";
    (*totalInBuffer)++;
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

                //cout<<"      Filled pageNo. at Page : "<<ph->GetPageNum()<<" Pos: " <<(*pos)<<endl;

                num = binMidPageNo;
                memcpy(&outputData[(*pos) * sizeof(int)], &num, sizeof(int));
                (*pos)++;

                if (*pos == numOfElementsInPage) {
                    *pos = 0;
                    outputFileFH->MarkDirty(ph->GetPageNum());
                    outputFileFH->UnpinPage(ph->GetPageNum());
                    // outputFileFH.FlushPage(ph.GetPageNum());
                    newPageH = outputFileFH->NewPage();
                    *ph = newPageH;
                    outputData = ph->GetData();

                }

                //cout<<"      Filled offset at Page : "<<ph->GetPageNum()<<" Pos: " <<(*pos)<<endl;

                num = i;
                memcpy(&outputData[(*pos) * sizeof(int)], &num, sizeof(int));
                (*pos)++;

            }

        }
    }

    //Unpinnng input page after exploring
    (*totalInBuffer)--;
    inputFileFH->UnpinPage(inputPageHandler.GetPageNum());

    if(firstHalf) {
        int newBinLastPageNo = binMidPageNo - 1;
        binarySearchAndAddtoOutput(s,binFirstPageNo,newBinLastPageNo,inputFileFH,outputFileFH,totalInBuffer,pos,ph);

    }

    if(lastHalf) {
        int newBinFirstPageNo = binMidPageNo + 1;
        binarySearchAndAddtoOutput(s,newBinFirstPageNo,binLastPageNo,inputFileFH,outputFileFH,totalInBuffer,pos,ph);

    }


     return;
    }

int main(int argc, char *argv[]){

    FileManager fm;
    int last_value;

    //cout<<"Opening file "<<argv[1]<<endl;
    FileHandler inputFileFH = fm.OpenFile(argv[1]);
    //cout << "Input file opened " << endl;


    //cout<<"Creating file "<<argv[3]<<endl;
    FileHandler outputFileFH= fm.CreateFile(argv[3]);
    //cout << "Output file created " << endl;


    string line;
    ifstream query_file(argv[2]);


    int* totalInBuffer = (int*)malloc(sizeof(int));
    int* pos = (int*)malloc(sizeof(int));
    *totalInBuffer = 0;
    *pos=0;

    //Calling Output Page to Memory Buffer
    (*totalInBuffer)++;
    PageHandler *ph = (PageHandler*)malloc(sizeof(PageHandler));
    PageHandler newPageH;
    newPageH = outputFileFH.NewPage();
    *ph = newPageH;

    char *outputData;
    int binFirstPageNo , binLastPageNo;


    //set the first/last and middle page no.s
    binLastPageNo = inputFileFH.LastPage().GetPageNum();

    //LastPage,FirstPage,PageAt,NextPage,PrevPAge  pins the pages hence
    inputFileFH.UnpinPage(binLastPageNo);


    //if input file is empty
    if(binLastPageNo == -1){
        int offset = 0;
        int end = -1;
        outputData = ph->GetData();
        while(getline(query_file, line)){

            memcpy(&outputData[offset*sizeof(int)], &end, sizeof(int));
            memcpy(&outputData[(offset+1)*sizeof(int)], &end, sizeof(int));
            offset += 2;

            if(offset == numOfElementsInPage){
                outputFileFH.MarkDirty(ph->GetPageNum());
                outputFileFH.UnpinPage(ph->GetPageNum());
                *ph = outputFileFH.NewPage();
                outputData = ph->GetData();
                offset = 0;
            }

        }

        if(offset != 0){
            int num = INT_MIN;
            while(offset != numOfElementsInPage){
                memcpy(&outputData[offset*sizeof(int)], &num, sizeof(int));
                offset++;
            }
            outputFileFH.MarkDirty(ph->GetPageNum());
            outputFileFH.UnpinPage(ph->GetPageNum());
        }
        

        outputFileFH.FlushPages();
        return 0;
}
    //set the first/last and middle page no.s
    binFirstPageNo = inputFileFH.FirstPage().GetPageNum(); 

    //LastPage,FirstPage,PageAt,NextPage,PrevPAge  pins the pages hence
    inputFileFH.UnpinPage(binFirstPageNo);


    //Look for every search Key
    while (getline(query_file, line)){


        int ps = line.find(' ');
        int s = stoi((line).substr(ps));



        //printf("\nMaincode: Search Key = %d , First = %d   Last= %d  Page No.s  ",s,binFirstPageNo,binLastPageNo);

        //Look for Pages with a search Key

        binarySearchAndAddtoOutput(s,binFirstPageNo,binLastPageNo,&inputFileFH,&outputFileFH,totalInBuffer,pos,ph);


        //Filling (-1,-1) as Last Entry
         //safety Check
        if (*pos == numOfElementsInPage){
            *pos = 0;
            outputFileFH.MarkDirty(ph->GetPageNum());
            outputFileFH.UnpinPage(ph->GetPageNum());
            newPageH = outputFileFH.NewPage();
            *ph = newPageH;
        }
        outputData = ph->GetData();
        //cout<<"\n Last Entry: "<<-1<<-1<<endl;
        //cout<<"      Filled -1 at Page : "<<ph->GetPageNum()<<" Pos: " <<(*pos)<<endl;
        last_value = -1;
        memcpy (&outputData[(*pos)*sizeof(int)], &last_value, sizeof(int));
        (*pos)++;

        if (*pos == numOfElementsInPage){
            *pos = 0;
            outputFileFH.MarkDirty(ph->GetPageNum());
            outputFileFH.UnpinPage(ph->GetPageNum());
            newPageH = outputFileFH.NewPage();
            *ph = newPageH;
        }
        outputData = ph->GetData();
        last_value = -1;
        //cout<<"      Filled -1 at Page : "<<ph->GetPageNum()<<" Pos: " <<(*pos)<<endl;
        memcpy (&outputData[(*pos)*sizeof(int)], &last_value, sizeof(int));
        (*pos)++;
        //cout<<"Pos in OutPut Page = "<<(*pos)<<endl;

    }

    //Filling INT_MIN as at empty pos
    last_value = INT_MIN;
    //cout<<"Pos in OutPut Page = "<<(*pos)<<endl;
    while(*pos < numOfElementsInPage){
        //cout<<"Fill remaining";
        memcpy (&outputData[(*pos)*sizeof(int)], &last_value, sizeof(int));
        (*pos)++;
    }

    outputFileFH.MarkDirty(ph->GetPageNum());
    outputFileFH.UnpinPage(ph->GetPageNum());
    outputFileFH.FlushPages();


    fm.CloseFile(inputFileFH);
    fm.CloseFile(outputFileFH);

}