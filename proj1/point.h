#include <vector>
#include <stdint.h>
#include <cstddef>
#include <iostream>
#include <chrono>
#include <ctime>
#ifndef POINT_H
#define POINT_H
class point
{
  public:
    std::vector<float> values;
    point(uint64_t nDim, float * vals);
    point();
    //point(point &&);
    point(const point &);

};


#endif
