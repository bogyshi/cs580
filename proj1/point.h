#include <vector>
#include <stdint.h>
#include <cstddef>
#ifndef POINT_H
#define POINT_H
class point
{
  public:
    uint64_t numDim;
    std::vector<float> values;
    point(uint64_t nDim, float * vals);
    point();
    ~point();

};
#endif
