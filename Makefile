CPP=g++
CPPOPT=-Wall -std=c++11
#CPPOPT=-Wall -std=c++14 # If C++14 is available
#CPPOPT=-Wall -std=c++11 -g # For debug

default: othello_match

othello_match: othello_match.cpp othello_ai.hpp
	$(CPP) $(CPPOPT) -o $@ $<

clean:
	rm -f othello_match othello_match.exe
