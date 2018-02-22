#ifndef READING_H
#define READING_H
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <thread>
#include "point.h"
#include "KDTree.h"
#include <cmath>
std::vector<point> readInput(std::string filename,std::uint64_t numCores);
void readQueries(std::string filename,std::uint64_t numCores,KDTree *);
void writeResults(std::string filename);
void writeBinary(std::string filename);
void readPoints(std::uint64_t offset,std::uint64_t numPoints,std::string);
std::vector<point> getKNearestNeighbors(point query);
float calcDist(point p1, point p2);
std::vector<point> getKNearestNeighbors(point query);
std::vector< std::pair<float,int>> getMinPoints(std::vector<point> og,point query);
std::vector<point> recursiveKNN(KDTree * node, uint64_t DS, std::vector<point> currPoints,point query);

#endif
