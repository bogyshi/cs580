#ifndef READING_H
#define READING_H
#include <iostream>
#include <fstream>
#include <string>
#include "point.h"
std::vector<point> readInput(std::string filename,std::uint64_t numCores);
void readQueries(std::string filename,std::uint64_t numCores);
void writeResults(std::string filename);
void writeBinary(std::string filename);
void readPoints(std::uint64_t offset,std::uint64_t numPoints);
#endif
