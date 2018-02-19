#include <vector>
#include <stdint.h>
#include <cstddef>
#ifndef POINT_H
#define POINT_H
class point
{
  public:
    uint64_t numDim;
    std::vector<uint32_t> values;
    point(uint64_t nDim, uint32_t * vals);
    point();
    ~point();

};
#endif
