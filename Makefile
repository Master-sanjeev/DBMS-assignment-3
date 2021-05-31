sampleobjects = buffer_manager.o file_manager.o sample_run.o
linearsearchobjects = buffer_manager.o file_manager.o linearsearch.o
binarysearchobjects = buffer_manager.o file_manager.o binarysearch.o
deletionobjects = buffer_manager.o file_manager.o deletion.o
join1objects = buffer_manager.o file_manager.o join1.o
join2objects = buffer_manager.o file_manager.o join2.o

sample_run : $(sampleobjects)
	     g++ -std=c++11  -g -o sample_run $(sampleobjects)

linearsearch : $(linearsearchobjects)
	g++ -std=c++11  -g -o linearsearch $(linearsearchobjects)


binarysearch : $(binarysearchobjects)
	g++ -std=c++11  -g -o binarysearch $(binarysearchobjects)

deletion : $(deletionobjects)
	g++ -std=c++11  -g -o deletion $(deletionobjects)

join1 : $(join1objects)
	g++ -std=c++11  -g -o join1 $(join1objects)


join2 : $(join2objects)
	g++ -std=c++11  -g -o join2 $(join2objects)

sample_run.o : sample_run.cpp
	g++ -std=c++11  -g -c sample_run.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11  -g -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11  -g -c file_manager.cpp

linearsearch.o : linearsearch.cpp
	g++ -std=c++11  -g -c linearsearch.cpp

binarysearch.o : binarysearch.cpp
	g++ -std=c++11  -g -c binarysearch.cpp

deletion.o : deletion.cpp
	g++ -std=c++11  -g -c deletion.cpp

join1.o : join1.cpp
	g++ -std=c++11  -g -c join1.cpp

join2.o : join2.cpp
	g++ -std=c++11  -g -c join2.cpp

clean :
	rm -f *.o
	rm -f sample_run
	rm -f linearsearch
	rm -f binarysearch
	rm -f join1
	rm -f join2
	rm -f deletion

