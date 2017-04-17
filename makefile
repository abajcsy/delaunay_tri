# Makefile for delaunay triangulation code

CC=gcc
CXX=g++
CFLAGS=-g -Wall

delaunay: delaunay.o edge.o predicates.o
	$(CXX) $(CFLAGS) -o delaunay quadedge.o  

delaunay.o: delaunay.cpp predicates.h edge.h
	$(CXX) $(CFLAGS) -c delaunay.cpp 

edge.o: edge.cpp edge.h
	$(CXX) $(CFLAGS) -c edge.cpp 

predicates.o: predicates.c
	$(CC) $(CFLAGS) -c predicates.c -o predicates.o 

clean: 
	rm *.o delaunay
