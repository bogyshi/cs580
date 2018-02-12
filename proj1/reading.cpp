#include "reading.h"
#include "KDTree.h"
using namespace std;

void readInput(string filename)
{
  ifstream file (filename, ios::in|ios::binary);
  uint64_t ID;
  file.read(reinterpret_cast<char *>(&ID),sizeof(ID));
  uint64_t numEntries;
  file.read(reinterpret_cast<char *>(&numEntries),sizeof(numEntries));
  uint64_t numDim;
  file.read(reinterpret_cast<char *>(&numDim),sizeof(numDim));
  uint32_t x1;
  uint32_t z1;
  uint32_t y1;
  uint32_t w1;
  printf("Header: %lu %lu %lu\n",ID,numEntries,numDim);
  while(!file.eof())
  {
    x1 = 1;
    file.read(reinterpret_cast<char *>(&x1),sizeof(x1));
    y1 = 2;
    file.read(reinterpret_cast<char *>(&y1),sizeof(y1));
    z1 = 3;
    file.read(reinterpret_cast<char *>(&z1),sizeof(z1));
    w1 = 4;
    file.read(reinterpret_cast<char *>(&w1),sizeof(w1));
    printf("Point: %u %u %u %u\n",x1,y1,z1,w1);
  }

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
