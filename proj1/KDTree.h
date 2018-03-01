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
#include <memory>

//#include <shared_mutex>

class KDTree
{
  public:
    uint64_t splitDim;
    std::unique_ptr<KDTree> left;
    std::unique_ptr<KDTree> right;
    KDTree();
    KDTree(uint64_t dimSplit);
    std::vector<point> allPoints;
};

std::unique_ptr<KDTree> buildTree(std::vector<point>,uint64_t numCores);
float sampledMedian(std::vector<point> points, uint64_t DTS);
void completeTree(uint64_t numDim);
void testKNN(std::vector<point> allPoints, point query, uint64_t kNN);
#endif
