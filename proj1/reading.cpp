#include "reading.h"
#include "KDTree.h"
#include "point.h"
#include <assert.h>
#include <thread>
using namespace std;

string inputFile;
string queryFile;
uint64_t inputID;
uint64_t queryID;
uint64_t numQueries;
uint64_t numDimensions;
uint64_t kNum;

bool debug = 0;

void readInput(string filename,const uint64_t numCores)
{
  inputFile = filename;
  ifstream file (inputFile, ios::in|ios::binary);
  char type[8];
  file.read(reinterpret_cast<char *>(&type),sizeof(type));
  file.read(reinterpret_cast<char *>(&inputID),sizeof(inputID));
  uint64_t numEntries;
  file.read(reinterpret_cast<char *>(&numEntries),sizeof(numEntries));
  uint64_t numDim;
  file.read(reinterpret_cast<char *>(&numDim),sizeof(numDim));
  printf("Header: %s %lu %lu %lu\n",type,inputID,numEntries,numDim);
  static char inputFileoffset = 32;//this is to let us seek past the pointless stuff in the beginning
  uint64_t numPoints = numEntries/numCores;
  uint64_t offsetData = numPoints*4*numDim;//4 is the size for each 32 bit in a points value, times the number of dimensions
  thread readers[numCores];
  uint64_t numThreads = 0;
  if(debug == 0){
    printf("Using %lu threads, we assign each thread %lu points, leaving the last thread with %lu entries.\n",numCores,numPoints, numEntries-numPoints*(numCores-1));
  }
  while(numThreads<numCores-1)//leave remaining points to last thread
  {
    readers[numThreads]= thread(readPoints,inputFileoffset+numThreads*offsetData,numPoints);
    numThreads++;
  }
  readers[numThreads]=thread(readPoints,inputFileoffset+numThreads*offsetData,numEntries-numPoints*numThreads);//these are the remaining points

  numThreads=0;
  while (numThreads<numCores)
  {
    readers[numThreads].join();
    numThreads++;
  }
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

  if(debug == 0){
    printf("Header repeat: %lu %lu\n",points.size(),points[0].numDim);
  }
  //printf("Point: %u %u\n",points[0].values[0],points[0].values[1]);
  //delete &points;
}

void readPoints(uint64_t offset,uint64_t numPoints)
{

}

void readQueries(string filename)
{
  ifstream file (filename, ios::in|ios::binary);
  char type[8]; // may want to generalize this;
  file.read(reinterpret_cast<char *>(&type),sizeof(type));
  type[5]='\0';
  file.read(reinterpret_cast<char *>(&queryID),sizeof(queryID));
  file.read(reinterpret_cast<char *>(&numQueries),sizeof(numQueries));
  file.read(reinterpret_cast<char *>(&numDimensions),sizeof(numDimensions));
  file.read(reinterpret_cast<char *>(&kNum),sizeof(kNum));
  printf("Header: %s %lu %lu %lu %lu\n",type,queryID,numQueries,numDimensions,kNum);
  //uint32_t test;
  //file.read(reinterpret_cast<char *>(&test),sizeof(&test));
  //printf("first: %u\n",test);
  vector<point> points;
  while(!file.eof())
  {
    uint32_t vals[numDimensions];
    file.read(reinterpret_cast<char *>(&vals),sizeof(&vals));
    if(file.ios::fail())
    {
      break;
    }
    points.push_back((point(numDimensions,vals))); //IS THIS ALLOWED?!??!?!?!!?
    //printf("Point: %u %u\n",vals[0],vals[1]);
  }

  if(debug == 0){
    printf("Header repeat: %lu %lu\n",points.size(),points[0].numDim);
  }
  //printf("Point: %u %u\n",points[0].values[0],points[0].values[1]);
  //delete &points;
}
void writeResults(string filename)
{

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
