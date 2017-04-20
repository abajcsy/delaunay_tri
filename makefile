# Makefile for delaunay triangulation code

CC = gcc
CXX = g++
CFLAGS = -g -Wall

delaunay: delaunay.o edge.o quadedge.o predicates.o vertex.o
	$(CXX) $(CFLAGS) -o delaunay delaunay.o edge.o quadedge.o predicates.o vertex.o

delaunay.o: delaunay.cpp predicates.h edge.h quadedge.h vertex.h
	$(CXX) $(CFLAGS) -c delaunay.cpp -o delaunay.o

quadedge.o: quadedge.cpp quadedge.h edge.h 
	$(CXX) $(CFLAGS) -c quadedge.cpp -o quadedge.o 

edge.o: edge.cpp edge.h quadedge.h vertex.h
	$(CXX) $(CFLAGS) -c edge.cpp -o edge.o 

vertex.o: vertex.cpp vertex.h
	$(CXX) $(CFLAGS) -c vertex.cpp -o vertex.o

predicates.o: predicates.c
	$(CC) $(CFLAGS) -c predicates.c -o predicates.o 

clean: 
	rm *.o delaunay
