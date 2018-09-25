CXX=gcc
CPPFLAGS=-mrdrnd -pthread

a: mt19937ar.c main.c
	$(CXX) $^ $(CPPFLAGS)
