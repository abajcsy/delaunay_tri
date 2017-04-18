# Makefile for delaunay triangulation code

CC=gcc
CXX=g++
CFLAGS=-g -Wall

delaunay: delaunay.o edge.o quadedge.o predicates.o
	$(CXX) $(CFLAGS) -o delaunay delaunay.o  

delaunay.o: delaunay.cpp predicates.h edge.h quadedge.h
	$(CXX) $(CFLAGS) -c delaunay.cpp 

quadedge.o: quadedge.cpp quadedge.h edge.h 
	$(CXX) $(CFLAGS) -c quadedge.cpp 

edge.o: edge.cpp edge.h quadedge.h
	$(CXX) $(CFLAGS) -c edge.cpp 

predicates.o: predicates.c
	$(CC) $(CFLAGS) -c predicates.c -o predicates.o 

clean: 
	rm *.o delaunay
