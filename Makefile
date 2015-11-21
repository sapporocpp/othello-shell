CPP=g++
CPPOPT=-Wall -std=c++11
#CPPOPT=-Wall -std=c++14 # If C++14 is available
#CPPOPT=-Wall -std=c++11 -g # For debug

default: othello_match

.cpp.o:
	$(CPP) $(CPPOPT) -c -o $@ $<

source/othello_ai.cpp: source/othello_ai.hpp

othello_match: source/othello_match.o source/othello_ai.o
	$(CPP) $(CPPOPT) -o $@ $^

clean:
	rm -vf othello_match othello_match.exe source/*.o
