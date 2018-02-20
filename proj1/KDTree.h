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

class KDTree
{
  public:
    uint64_t splitDim;
    uint64_t numPoints;
    KDTree * left;
    KDTree * right;
    KDTree();
    KDTree(uint64_t dimSplit);
    ~KDTree();
    std::vector<point> allPoints;
};

KDTree * buildTree(std::vector<point>,uint64_t numCores);
uint32_t sampledMedian(std::vector<point> points, uint64_t DTS);
#endif
