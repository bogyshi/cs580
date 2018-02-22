#ifndef KDTREE_H
#define KDTREE_H
#include <vector>
#include "point.h"
#include <cstdlib>
#include <random>
#include <algorithm>
#include <functional>
#include <queue>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <chrono>

//#include <shared_mutex>

class KDTree
{
  public:
    uint64_t splitDim;
    uint64_t numPoints;
    uint64_t depth;
    KDTree * left;
    KDTree * right;
    KDTree();
    KDTree(uint64_t dimSplit);
    ~KDTree();
    std::vector<point> allPoints;
};

KDTree * buildTree(std::vector<point>,uint64_t numCores);
float sampledMedian(std::vector<point> points, uint64_t DTS);
void completeTree(uint64_t numDim);
#endif
