#include "reading.h"
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

vector<point> readInput(string filename,const uint64_t numCores)
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

  //uint32_t test;
  //file.read(reinterpret_cast<char *>(&test),sizeof(&test));
  //printf("first: %u\n",test);
  vector<point> points;
  while(!file.eof())
  {
    float vals[numDim];
    file.read(reinterpret_cast<char *>(&vals),sizeof(&vals));
    if(file.ios::fail())
    {
      break;
    }
    points.push_back((point(numDim,vals))); //IS THIS ALLOWED?!??!?!?!!?
    //printf("Point: %f %f\n",vals[0],vals[1]);
  }

  if(debug == 0){
    printf("Header repeat: %lu %f\n",points.size(),points[0].values[1]);
  }
  return points;
  //printf("Point: %u %u\n",points[0].values[0],points[0].values[1]);
  //delete &points;
}

void readPoints(uint64_t offset,uint64_t numPoints)
{

}

void readQueries(string filename, uint64_t numCores)
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
  static char inputFileoffset = 40;//this is to let us seek past the pointless stuff in the beginning
  uint64_t querySize = numDimensions*4;
  uint64_t numIters = numQueries/numCores;
  uint64_t offsetData = numIters*querySize;//4 is the size for each 32 bit in a points value, times the number of dimensions
  thread readers[numCores];
  uint64_t numThreads = 0;
  if(debug == 0){
    printf("Using %lu threads, we assign each thread %lu points, leaving the last thread with %lu entries.\n",numCores,numIters, numQueries-numIters*(numCores-1));
  }
  while(numThreads<numCores-1)//leave remaining points to last thread
  {
    readers[numThreads]= thread(readPoints,inputFileoffset+numThreads*offsetData,numIters);
    numThreads++;
  }
  readers[numThreads]=thread(readPoints,inputFileoffset+numThreads*offsetData,numQueries-numIters*numThreads);//these are the remaining points

  numThreads=0;
  while (numThreads<numCores)
  {
    readers[numThreads].join();
    numThreads++;
  }

  vector<point> points;
  while(!file.eof())
  {
    float vals[numDimensions];
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
  char type[8] = "TRAIN";
  uint64_t ID = 1234;
  uint64_t numEntries = 100;
  uint64_t numDim = 4;
  uint32_t x1 = 1;
  uint32_t y1 = 2;
  uint32_t z1 = 3;
  uint32_t w1 = 4;

  file.open(filename,ios::out|ios::binary);
  file.write((char *) &type,sizeof(type));
  file.write((char *) &ID,sizeof(ID));
  file.write((char *) &numEntries,sizeof(numEntries));
  file.write((char *) &numDim,sizeof(numDim));
  file.write((char *) &x1,sizeof(x1));
  file.write((char *) &y1,sizeof(y1));
  file.write((char *) &z1,sizeof(z1));
  file.write((char *) &w1,sizeof(w1));
}
