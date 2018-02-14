#include "reading.h"
#include "KDTree.h"
#include "point.h"
#include <assert.h>
using namespace std;

void readInput(string filename)
{
  ifstream file (filename, ios::in|ios::binary);
  char type[8];
  file.read(reinterpret_cast<char *>(&type),sizeof(type));
  uint64_t ID;
  file.read(reinterpret_cast<char *>(&ID),sizeof(ID));
  uint64_t numEntries;
  file.read(reinterpret_cast<char *>(&numEntries),sizeof(numEntries));
  uint64_t numDim;
  file.read(reinterpret_cast<char *>(&numDim),sizeof(numDim));
  printf("Header: %s %lu %lu %lu\n",type,ID,numEntries,numDim);
  //uint32_t test;
  //file.read(reinterpret_cast<char *>(&test),sizeof(&test));
  //printf("first: %u\n",test);
  vector<point> points;
  while(!file.eof())
  {
    uint32_t vals[numDim];
    file.read(reinterpret_cast<char *>(&vals),sizeof(&vals));
    if(file.ios::fail())
    {
      break;
    }
    points.push_back((point(numDim,vals))); //IS THIS ALLOWED?!??!?!?!!?
    //printf("Point: %u %u\n",vals[0],vals[1]);
  }
  printf("Header repeat: %lu %lu\n",points.size(),points[0].numDim);
  //printf("Point: %u %u\n",points[0].values[0],points[0].values[1]);
  //delete &points;
}

void writeBinary(string filename)
{
  ofstream file;
  uint64_t ID = 1234;
  uint64_t numEntries = 100;
  uint64_t numDim = 4;
  uint32_t x1 = 1;
  uint32_t y1 = 2;
  uint32_t z1 = 3;
  uint32_t w1 = 4;
  file.open(filename,ios::out|ios::binary);
  file.write((char *) &ID,sizeof(ID));
  file.write((char *) &numEntries,sizeof(numEntries));
  file.write((char *) &numDim,sizeof(numDim));
  file.write((char *) &x1,sizeof(x1));
  file.write((char *) &y1,sizeof(y1));
  file.write((char *) &z1,sizeof(z1));
  file.write((char *) &w1,sizeof(w1));
}
