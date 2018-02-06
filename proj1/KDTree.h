#ifndef KDTREE_H
#define KDTREE_H
#include <vector>
class KDTree
{
  public:
    uint64_t numDim;
    uint64_t numPoints;
    std::vector<std::vector<uint32_t> > allPoints;
};

#endif
