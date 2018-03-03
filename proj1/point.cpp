#include "point.h"
#include <cstring>
point::point(uint64_t nDim, float * vals)
{
  unsigned int i = 0;
  for (;i<nDim;i++)
  {
    values.push_back(vals[i]);

  }
  //values(vals,vals+nDim);
  //std::memset(values,0,numDim);
  //std::memcpy(values,vals,sizeof(uint32_t)*nDim)
}
point::point()
{


}
point::point(const point &p)
{
  for (float x: p.values)
  {
    values.push_back(std::move(x));
  }
}
