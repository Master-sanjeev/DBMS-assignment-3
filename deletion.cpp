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

	//cout <<"Buffer size is "<<BUFFER_SIZE << endl;
    FileManager fm;

	//cout<<"Opening file "<<argv[1]<<endl;
	FileHandler inputFileFH = fm.OpenFile(argv[1]);
	//cout << "Input file opened " << endl;

	//cout<<"Opening file "<<argv[1]<<endl;
	FileHandler inputFileFH2 = fm.OpenFile(argv[1]);
	//cout << "Input file opened " << endl;

	// //cout<<"Opening file "<<argv[2]<<endl;
	// FileHandler queryFileFH = fm.OpenFile(argv[2]);
	// //cout << "Query file opened " << endl;
	int numOfElementsInPage = PAGE_CONTENT_SIZE / sizeof(int);
	
	string line;
	ifstream query_file(argv[2]);

	int i = 0, j = 0;
	int pageI = 0, pageJ = 0;

	int num1, num2;

	int lastInputPageNum = inputFileFH.LastPage().GetPageNum();
	//cout<<"Last page number is "<<lastInputPageNum<<endl;
	inputFileFH.UnpinPage(lastInputPageNum);

	fm.CloseFile(inputFileFH);
	fm.CloseFile(inputFileFH2);

	if(lastInputPageNum==-1){
		return 0;
}


	

	while(getline(query_file, line)){

		inputFileFH = fm.OpenFile(argv[1]);
		inputFileFH2 = fm.OpenFile(argv[1]);
		int ps = line.find(' ');
		int del = stoi((line).substr(ps));
		
		int q1 = -1, offset1 = -1;
		//cout<<"Deleting "<<del<<endl;
		for (int pageNum = 0; pageNum <= lastInputPageNum; pageNum++){
			PageHandler paheHandler1 = inputFileFH.PageAt(pageNum);
			char *data1 = paheHandler1.GetData();
			for (int i = 0; i < numOfElementsInPage; i++){
				
				memcpy (&num1, &data1[i*sizeof(int)], sizeof(int));

				if (num1 == del){
					// //cout<<"haha "<<pageNum<<endl;
					q1 = pageNum;
					offset1 = i;
					inputFileFH.UnpinPage(pageNum);
					break;
				}

			}

			inputFileFH.UnpinPage(pageNum);
			if (q1 != -1){
				break;
			}
		}
		//cout<<"q1 = "<<q1<<"; offset1 = "<<offset1<<endl;
		if (q1 == -1){
			continue;
		}

		int q2 = -1, offset2 = -1;

		for (int pageNum = lastInputPageNum; pageNum >= q1; pageNum--){
			PageHandler paheHandler1 = inputFileFH.PageAt(pageNum);
			char *data1 = paheHandler1.GetData();
			for (int i = numOfElementsInPage-1; i>=0; i--){
				memcpy (&num1, &data1[i*sizeof(int)], sizeof(int));

				if (num1 == del){
					offset2 = i;
					q2 = pageNum;
					break;
				}

			}
			inputFileFH.UnpinPage(pageNum);

			if (q2 != -1){
				break;
			}

		}

		//cout<<"q1 = "<<q1<<"; offset1 = "<<offset1<<"; q2 = "<<q2<<"; offset2 = "<<offset2<<endl;

		

		if (offset2 == numOfElementsInPage-1){
			offset2 = 0;
			q2 ++;
		} else {
			offset2++;
		}

		//cout<<"after incrm q1 = "<<q1<<"; offset1 = "<<offset1<<"; q2 = "<<q2<<"; offset2 = "<<offset2<<endl;

		PageHandler pageHandler1 = inputFileFH.PageAt(q1);
		char *data1 = pageHandler1.GetData();

		char* data2;

		PageHandler pageHandler2;

		if (q2 <= lastInputPageNum){

			pageHandler2 = inputFileFH2.PageAt(q2);
			data2 = pageHandler2.GetData();

		}

		while(1){

			if (q2 > lastInputPageNum){
				break;
			}

			memcpy (&num1, &data1[offset1*sizeof(int)], sizeof(int));
			memcpy (&num2, &data2[offset2*sizeof(int)], sizeof(int));

			//cout<<num1 <<" "<<num2<<endl;

			if (num2 == INT_MIN){
				// inputFileFH.MarkDirty(q1);

				// inputFileFH2.MarkDirty(q2);
				break;
			}
			
			memcpy (&data1[offset1*sizeof(int)], &num2, sizeof(int));
			// inputFileFH.MarkDirty(q1);
			int tmp;
			memcpy (&tmp, &data1[offset1*sizeof(int)], sizeof(int));

			// memcpy (&data2[offset2*sizeof(int)], &num1, sizeof(int));
			//cout<<"dsa"<<tmp <<" "<<num2<<endl;

			offset1++;
			offset2++;

			if (offset1 == numOfElementsInPage){
				offset1 = 0;
				inputFileFH.MarkDirty(q1);
				inputFileFH.UnpinPage(q1);
				q1 ++;
				if (q1 <= lastInputPageNum){
					pageHandler1 = inputFileFH.PageAt(q1);
					data1 = pageHandler1.GetData();
				}
			}

			if (offset2 == numOfElementsInPage){
				offset2 = 0;
				inputFileFH2.MarkDirty(q2);
				inputFileFH2.UnpinPage(q2);
				q2 ++;
				if (q2 <= lastInputPageNum){
					pageHandler2 = inputFileFH2.PageAt(q2);
					data2 = pageHandler2.GetData();
				}
			}
			// inputFileFH.FlushPages();


		}

		inputFileFH.MarkDirty(pageHandler1.GetPageNum());
		inputFileFH.UnpinPage(pageHandler1.GetPageNum());
		if (pageHandler1.GetPageNum() != pageHandler2.GetPageNum()){
			inputFileFH2.MarkDirty(pageHandler2.GetPageNum());
			inputFileFH2.UnpinPage(pageHandler2.GetPageNum());
		}

		inputFileFH.FlushPages();
		inputFileFH2.FlushPages();

		num1 = INT_MIN;

		//cout<<"Setting INT_MIN from "<<q1 << ", "<<offset1<<endl;

		while(1){

			if (q1 > lastInputPageNum){
				break;
			}
			
			pageHandler1 = inputFileFH.PageAt(q1);
			data1 = pageHandler1.GetData();
			memcpy (&data1[offset1*sizeof(int)], &num1, sizeof(int));
			inputFileFH.MarkDirty(q1);
			offset1++;

			if (offset1 == numOfElementsInPage){
				offset1 = 0;
				inputFileFH.MarkDirty(q1);
				inputFileFH.UnpinPage(q1);
				q1++;
			}

		}


		inputFileFH.FlushPages();

		fm.CloseFile(inputFileFH);
		fm.CloseFile(inputFileFH2);

	}

	inputFileFH = fm.OpenFile(argv[1]);

	for (int pageNum = 0; pageNum <= lastInputPageNum; pageNum++){
		int tmp;
		PageHandler ph = inputFileFH.PageAt(pageNum);
		char *data1 = ph.GetData();
		memcpy (&tmp, &data1[0], sizeof(int));

		if (tmp == INT_MIN){
			inputFileFH.DisposePage(pageNum);
		}

		inputFileFH.UnpinPage(pageNum);
	}

	fm.CloseFile(inputFileFH);
	

}