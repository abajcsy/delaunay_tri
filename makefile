# Makefile for delaunay triangulation code

CC=gcc
CXX=g++

quadtest: quadedge.o
	$(CXX) quadedge.o -o quadtest 

quadedge.o: quadedge.cpp predicates.h edge.h
	$(CXX) quadedge.cpp edge.cpp -o quadedge.o 

edge.o: edge.cpp
	$(CXX) edge.cpp -o edge.o 

predicates.o: predicates.c
	$(CC) -c predicates.c -o predicates.o 

clean: 
	rm *.o quadtest
