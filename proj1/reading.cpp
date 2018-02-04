#include "reading.h"

using namespace std;

void readInput(string filename)
{
  ifstream file ("test", ios::in|ios::binary);
  uint64_t ID;
  file.read(reinterpret_cast<char *>(&ID),sizeof(ID));
  uint64_t numEntries;
  file.read(reinterpret_cast<char *>(&numEntries),sizeof(numEntries));
  uint64_t numDim;
  file.read(reinterpret_cast<char *>(&numDim),sizeof(numDim));
  uint32_t x1 = 1;
  file.read(reinterpret_cast<char *>(&x1),sizeof(x1));
  uint32_t y1 = 2;
  file.read(reinterpret_cast<char *>(&y1),sizeof(y1));
  uint32_t z1 = 3;
  file.read(reinterpret_cast<char *>(&z1),sizeof(z1));
  uint32_t w1 = 4;
  file.read(reinterpret_cast<char *>(&w1),sizeof(w1));
  printf("Header: %lu %lu %lu",ID,numEntries,numDim);
}
