#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include<vector>
#include<fstream>
#include<cmath>
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char *argv[]){

    FileManager fm;

	//cout<<"Opening file "<<argv[1]<<endl;
	FileHandler inputFileFH = fm.OpenFile(argv[1]);
	//cout << "Input file opened " << endl;
	

	//cout<<"Creating file "<<argv[3]<<endl;
	FileHandler outputFileFH= fm.CreateFile(argv[3]);
	//cout << "Output file created " << endl;

	int numOfElementsInPage = PAGE_CONTENT_SIZE / sizeof(int);

	string line;
	ifstream query_file(argv[2]);

	int pos = 0;
	int num;
	int totalInBuffer = 0;
	totalInBuffer++;
	PageHandler ph = outputFileFH.NewPage ();
	char *outputData = ph.GetData();

	int lastInputPageNum = inputFileFH.LastPage().GetPageNum();
	//cout<<"Last page number is "<<lastInputPageNum<<endl;
	inputFileFH.UnpinPage(lastInputPageNum);

	if(lastInputPageNum == -1){
		int offset = 0;
		int end = -1;
		while(getline(query_file, line)){

			memcpy(&outputData[offset*sizeof(int)], &end, sizeof(int));
			memcpy(&outputData[(offset+1)*sizeof(int)], &end, sizeof(int));
			offset += 2;

			if(offset == numOfElementsInPage){
				outputFileFH.MarkDirty(ph.GetPageNum());
				outputFileFH.UnpinPage(ph.GetPageNum());
				ph = outputFileFH.NewPage();
				outputData = ph.GetData();
				offset = 0;
			}

		}

		if(offset != 0){
			int num = INT_MIN;
			while(offset != numOfElementsInPage){
				memcpy(&outputData[offset*sizeof(int)], &num, sizeof(int));
				offset++;
			}
			outputFileFH.MarkDirty(ph.GetPageNum());
			outputFileFH.UnpinPage(ph.GetPageNum());
		}
		

		outputFileFH.FlushPages();
		return 0;
	}

	while (getline(query_file, line)){

		int ps = line.find(' ');
		int s = stoi((line).substr(ps));
		//cout<<"============ Searching " << s << " ======================\n";
		int inputFilePageCount = 0;
		PageHandler inputPageHandler;

		while(1){

			// //cout<<"Total pinned are "<<totalInBuffer<<endl;
			//cout<<"Getting input page "<<inputFilePageCount<<" \n";
			totalInBuffer++;
			// inputFileFH.UnpinPage(inputFilePageCount);
			
			inputPageHandler = inputFileFH.PageAt(inputFilePageCount);	

			char *data = inputPageHandler.GetData();

			for(int i = 0; i<numOfElementsInPage; i++){

				memcpy (&num, &data[i*sizeof(int)], sizeof(int));
				// //cout<<num<<endl;
				if (num == s){
					// //cout<<"shivands \n";
					// //cout<<inputFilePageCount<<endl;
					// //cout<<i<<endl;

					if (pos == numOfElementsInPage){
						pos = 0;
						outputFileFH.MarkDirty(ph.GetPageNum());
						outputFileFH.UnpinPage(ph.GetPageNum());
						// outputFileFH.FlushPage(ph.GetPageNum());
						ph = outputFileFH.NewPage();
						outputData = ph.GetData();

					}

					num = inputFilePageCount;
					memcpy (&outputData[pos*sizeof(int)], &num, sizeof(int));
					pos++;

					if (pos == numOfElementsInPage){
						pos = 0;
						outputFileFH.MarkDirty(ph.GetPageNum());
						outputFileFH.UnpinPage(ph.GetPageNum());
						// outputFileFH.FlushPage(ph.GetPageNum());
						ph = outputFileFH.NewPage();
						outputData = ph.GetData();

					}

					num = i;
					memcpy (&outputData[pos*sizeof(int)], &num, sizeof(int));
					pos++;

				}

			}	
			totalInBuffer--;
			inputFileFH.UnpinPage(inputPageHandler.GetPageNum());
			// if (!inputFileFH.UnpinPage(inputFilePageCount)){
			// 	//cout<<"Input file page num "<<inputFilePageCount<<" unpinned failed\n";
			// }
			// inputFileFH.FlushPage(inputFilePageCount);

			if (inputFilePageCount == lastInputPageNum){
				break;
			}

			
			inputFilePageCount++;	


		}

		if (pos == numOfElementsInPage){
			pos = 0;
			outputFileFH.MarkDirty(ph.GetPageNum());
			outputFileFH.UnpinPage(ph.GetPageNum());
			ph = outputFileFH.NewPage();
			outputData = ph.GetData();

		}
		//cout<<-1<<endl;
		//cout<<-1<<endl;
		num = -1;
		memcpy (&outputData[pos*sizeof(int)], &num, sizeof(int));
		pos++;

		if (pos == numOfElementsInPage){
			pos = 0;
			outputFileFH.MarkDirty(ph.GetPageNum());
			outputFileFH.UnpinPage(ph.GetPageNum());
			ph = outputFileFH.NewPage();
			outputData = ph.GetData();

		}

		num = -1;
		memcpy (&outputData[pos*sizeof(int)], &num, sizeof(int));
		pos++;
		//cout<<"Pos = "<<pos<<endl;

    }
	num = INT_MIN;
	//cout<<"Pos = "<<pos<<endl;
	while(pos < numOfElementsInPage){
		//cout<<"haha ";
		memcpy (&outputData[pos*sizeof(int)], &num, sizeof(int));
		pos++;
	}
	
	outputFileFH.MarkDirty(ph.GetPageNum());
	outputFileFH.UnpinPage(ph.GetPageNum());

	outputFileFH.FlushPages();

	fm.CloseFile(inputFileFH);
	fm.CloseFile(outputFileFH);



}