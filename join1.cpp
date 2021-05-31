#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include<vector>
#include<fstream>
#include<cmath>
#include <bits/stdc++.h>
//#include "stdc++.h"

using namespace std;

int main(int argc, char *argv[]){

    FileManager fm;

	//cout<<"Opening file "<<argv[1]<<endl;
	FileHandler inputFileFH1 = fm.OpenFile(argv[1]);
	//cout << "Input file opened " << endl;

	//cout<<"Opening file "<<argv[2]<<endl;
	FileHandler inputFileFH2 = fm.OpenFile(argv[2]);
	//cout << "Input file opened " << endl;
	

	//cout<<"Creating file "<<argv[3]<<endl;
	FileHandler outputFileFH= fm.CreateFile(argv[3]);
	//cout << "Output file created " << endl;

	int numOfElementsInPage = PAGE_CONTENT_SIZE / sizeof(int);

	int pos = 0;
	int num;

	bool isEmptyOutput = true;

	PageHandler ph = outputFileFH.NewPage ();
	char *outputData = ph.GetData();

	int lastInput1PageNum = inputFileFH1.LastPage().GetPageNum();

	inputFileFH1.UnpinPage(lastInput1PageNum);

	int lastInput2PageNum = inputFileFH2.LastPage().GetPageNum();

	inputFileFH2.UnpinPage(lastInput2PageNum);
	int num1, num2;


	//If any of the file is empty then return an empty file
	if(lastInput1PageNum==-1 || lastInput2PageNum==-1){
		outputFileFH.DisposePage(ph.GetPageNum());
		outputFileFH.FlushPages();
		fm.CloseFile(inputFileFH1);
		fm.CloseFile(inputFileFH2);
		fm.CloseFile(outputFileFH);
		return 0;
	}


	char *data1;
	char *data2;
	
	//cout<<"last input 1 page num " << lastInput1PageNum << ";; last input 2 page num " << lastInput2PageNum<<endl;

	for (int i = 0; i <= lastInput1PageNum; i++){

		PageHandler pageHandler1 = inputFileFH1.PageAt(i);
		data1 = pageHandler1.GetData();

		for (int j = 0; j <= lastInput2PageNum; j++){

			PageHandler pageHandler2 = inputFileFH2.PageAt(j);
			
			data2 = pageHandler2.GetData();
			for (int k = 0; k < numOfElementsInPage; k++){
				memcpy (&num1, &data1[k*sizeof(int)], sizeof(int));
				
				for (int l = 0; l < numOfElementsInPage; l++){

					memcpy (&num2, &data2[l*sizeof(int)], sizeof(int));

					if (num1 == num2){

					    if(isEmptyOutput){
                            isEmptyOutput = false;
					    }

						if (pos == numOfElementsInPage){
							pos = 0;
							outputFileFH.MarkDirty(ph.GetPageNum());
							outputFileFH.UnpinPage(ph.GetPageNum());
							// outputFileFH.FlushPage(ph.GetPageNum());
							ph = outputFileFH.NewPage();
							outputData = ph.GetData();

						}

						num = num1;
						memcpy (&outputData[pos*sizeof(int)], &num, sizeof(int));
						pos++;

					}

				}

			}

			if (1){
				inputFileFH2.UnpinPage(j);
			}

		}

		inputFileFH1.UnpinPage(i);

	}

	num = INT_MIN;
	// //cout<<"Pos = "<<pos<<endl;
	while(pos < numOfElementsInPage){
		// //cout<<"haha ";
		memcpy (&outputData[pos*sizeof(int)], &num, sizeof(int));
		pos++;
	}

	if(isEmptyOutput) {
        outputFileFH.DisposePage(ph.GetPageNum());
	}
	else
        {
            outputFileFH.MarkDirty(ph.GetPageNum());
            outputFileFH.UnpinPage(ph.GetPageNum());
        }
	outputFileFH.FlushPages();

	fm.CloseFile(inputFileFH1);
	fm.CloseFile(inputFileFH2);
	fm.CloseFile(outputFileFH);


}
