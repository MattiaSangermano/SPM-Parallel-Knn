
CXX		= g++ -std=c++17

LDFLAGS 	= -pthread
OPTFLAGS	= -O3

TARGETS		= parallel
INC = ~/fastflow/
.PHONY: all clean
.SUFFIXES: .cpp 

sequential	: sequential.cpp 
	$(CXX) sequential.cpp utils.cpp $(OPTFLAGS)  -o $@ $(LDFLAGS)

parallel	: parallel.cpp 
	$(CXX) parallel.cpp utils.cpp $(OPTFLAGS)  -o $@ $(LDFLAGS)
ff_parallel: ff_parallel.cpp
	$(CXX) ff_parallel.cpp utils.cpp $(OPTFLAGS) -I $(INC) -o $@ $(LDFLAGS)
ff_parallel_for: ff_parallel_for.cpp
	$(CXX) ff_parallel_for.cpp utils.cpp $(OPTFLAGS) -I $(INC) -o $@ $(LDFLAGS)
fileCreation		:
	$(CXX) fileCreation.cpp $(OPTFLAGS)  -o $@ $(LDFLAGS)
threadTime		:
	$(CXX) threadTime.cpp $(OPTFLAGS)  -o $@ $(LDFLAGS)

clean		: 
	rm -f $(TARGETS) 



