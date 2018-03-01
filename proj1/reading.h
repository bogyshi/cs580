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
#include <mutex>
#include <malloc.h>
std::vector<point> readInput(std::string filename);
void readQueries(std::string filename,std::uint64_t numCores,KDTree *,std::string rname,std::vector<point>);
void writeBinary(std::string filename);
void readPoints(std::uint64_t offset,std::uint64_t numPoints,std::string,std::string,uint64_t,uint64_t);
std::vector<point> getKNearestNeighbors(point query);
float calcDist(point p1, point p2);
std::vector<point> getKNearestNeighbors(point query);
std::vector< std::pair<float,int>> getMinPoints(std::vector<point> og,point query);
std::vector<point> recursiveKNN(KDTree * node, uint64_t DS, std::vector<point> currPoints,point query);
void readResults(std::string rfileName);

#endif
