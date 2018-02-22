#include "reading.h"
using namespace std;

string inputFile;
string queryFile;
uint64_t inputID;
uint64_t queryID;
uint64_t numQueries;
uint64_t numDimensions;
uint64_t kNum;

bool debug = 0;

static KDTree * head;

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

void readPoints(uint64_t offset,uint64_t numPoints,string filename)
{
  uint64_t i = 0;
  uint64_t j=0;
  int dC = 0;
  float val;
  ifstream file (filename, ios::in|ios::binary);
  //cerr<<offset<<endl;
  file.seekg(offset);
  while(i<numPoints)
  {

    float vals[numDimensions];
    while(j<numDimensions)
    {
      file.read(reinterpret_cast<char *>(&val),sizeof(val));
      if(file.ios::fail())
      {
        break;
      }
      vals[j]=val;
      j++;
    }
    point query = point(numDimensions,vals);

    vector<point> knn = getKNearestNeighbors(query);
    /*printf("%f,%f with NN \n",query.values[0],query.values[1]);
    dC=0;
    while(dC<kNum)
    {
      float x = calcDist(query,knn[dC]);
      printf("%f,%f with dist %f\n",knn[dC].values[0],knn[dC].values[1],x);
      ++dC;
    }*/

    j=0;
    ++i;
  }
  cerr<<"handled "<<i<<" points";
}

float calcDist(point p1, point p2)
{
  uint64_t i = 0;
  double sumDist = 0.0;
  while(i<numDimensions)
  {
    sumDist+=pow(p1.values[i]-p2.values[i],2.0);
    i++;
  }
  if(sqrt(sumDist)<=0)
  {
    cerr<<"whut";
  }
  //cerr<<sumDist;
  return (float)sqrt(sumDist);
}

vector<point> getKNearestNeighbors(point query)
{
  KDTree * root = head;
  uint64_t DS=0;
  vector<point> og;
  return recursiveKNN(root,DS,og,query);

}

vector< pair<float,int>> getMinPoints(vector<point> og,point query)
{
  int i = 0;
  vector< pair<float,int>> SI;//sorted indexes
  while(i<og.size())
  {
    float temp = calcDist(query,og[i]);
    SI.push_back(pair<float,int>(temp,i));
    ++i;
  }
  sort(SI.begin(),SI.end());
  return SI;
}

vector<point> recursiveKNN(KDTree * node, uint64_t DS, vector<point> currPoints,point query)
{
  vector<point> newCurrPoints;
  if(node->allPoints[0].values[DS]<query.values[DS] && node->right !=NULL)
  {
    newCurrPoints = recursiveKNN(node->right,(DS+1)%numDimensions,currPoints,query);
    if(newCurrPoints.size()<kNum && node->left!=NULL)//we havent found enough neighbors yet! go down the other branch
    {
      newCurrPoints = recursiveKNN(node->left,(DS+1)%numDimensions,newCurrPoints,query);
    }
    else // check difference between splitDimension points, we have enough neighboors, but maybe there is more to go
    {
      if(query.values[DS] - node->allPoints[0].values[DS] < calcDist(query,newCurrPoints[kNum-1]))//distance means there could be a point closer on the other side!
      {
        if(node->left!=NULL)
        {
          newCurrPoints = recursiveKNN(node->left,(DS+1)%numDimensions,newCurrPoints,query);
        }
      }
    }
    if(calcDist(query,node->allPoints[0]) < calcDist(query,newCurrPoints[kNum-1])) //see if the point here is any good
    {
      int i = 0;
      while(i<newCurrPoints.size())
      {
        if(calcDist(query,node->allPoints[0]) < calcDist(query,newCurrPoints[i]))
        {
          newCurrPoints.insert(newCurrPoints.begin()+i,node->allPoints[0]);
          break;
        }
        ++i;
      }
      newCurrPoints.pop_back();
    }
  }
  else if(node->allPoints[0].values[DS]>query.values[DS] && node->left !=NULL)
  {
    newCurrPoints = recursiveKNN(node->left,(DS+1)%numDimensions,currPoints,query);
    if(newCurrPoints.size()<kNum && node->right!=NULL)
    {
      newCurrPoints = recursiveKNN(node->right,(DS+1)%numDimensions,newCurrPoints,query);
    }
    else // check difference between splitDimension points
    {
      if(query.values[DS] - node->allPoints[0].values[DS] < calcDist(query,newCurrPoints[kNum-1]))//distance means there could be a point closer on the other side!
      {
        if(node->right!=NULL)
        {
          newCurrPoints = recursiveKNN(node->right,(DS+1)%numDimensions,newCurrPoints,query);
        }
      }
    }
    if(calcDist(query,node->allPoints[0]) < calcDist(query,newCurrPoints[kNum-1]))
    {
      int i = 0;
      while(i<newCurrPoints.size())
      {
        if(calcDist(query,node->allPoints[0]) < calcDist(query,newCurrPoints[i]))
        {
          newCurrPoints.insert(newCurrPoints.begin()+i,node->allPoints[0]);
          break;
        }
        ++i;
      }
      newCurrPoints.pop_back();
    }
  }
  else if(node->left==NULL && node->right == NULL)
  {
    int i =0;
    int sz= node->allPoints.size();
    int sz2;
    vector< pair<float,int>> minPoints = getMinPoints(node->allPoints,query);
    if(currPoints.size()==0)
    {
      while(currPoints.size()<=kNum && i < node->allPoints.size())
      {
        //cerr<<"beg"<<minPoints[i].second<<"end"<<endl;
        newCurrPoints.push_back(node->allPoints[minPoints[i].second]);
        ++i;
      }
    }
    else//handles the case where we havent satisfied kNN and have some already.
    {
      sz2=currPoints.size();
      int x=0;
      int y=0;
      while(x<sz && y<sz2 && newCurrPoints.size()<kNum)
      {
        if(calcDist(query,node->allPoints[minPoints[x].second])<calcDist(query,currPoints[y]) )
        {
          newCurrPoints.push_back(node->allPoints[minPoints[x].second]);
          ++x;
        }
        else
        {
          newCurrPoints.push_back(currPoints[y]);
          ++y;
        }
      }
      if(newCurrPoints.size()==kNum)
      {

      }
      else if(x==sz && y<sz2)
      {
        while(y<sz2 && newCurrPoints.size()<kNum)
        {
          newCurrPoints.push_back(currPoints[y]);
          ++y;
        }
      }
      else if(y==sz2 && x<sz)
      {
        while(x<sz && newCurrPoints.size()<kNum)
        {
          newCurrPoints.push_back(node->allPoints[minPoints[x].second]);
          ++x;
        }
      }
    }

  }
  return newCurrPoints;

}
void readQueries(string filename, uint64_t numCores,KDTree * root)
{
  head = root;
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
  uint64_t numIters = numQueries/numCores;//how many we are going to do
  uint64_t offsetData = numIters*querySize;//4 is the size for each 32 bit in a points value, times the number of dimensions
  thread readers[numCores];
  uint64_t numThreads = 0;
  if(debug == 0){
    printf("Using %lu threads, we assign each thread %lu points, leaving the last thread with %lu entries.\n",numCores,numIters, numQueries-numIters*(numCores-1));
  }
  while(numThreads<numCores-1)//leave remaining points to last thread
  {
    readers[numThreads]= thread(readPoints,inputFileoffset+numThreads*offsetData,numIters,filename);
    numThreads++;
  }
  readers[numThreads]=thread(readPoints,inputFileoffset+numThreads*offsetData,numQueries-numIters*numThreads,filename);//these are the remaining points

  numThreads=0;
  while (numThreads<numCores)
  {
    readers[numThreads].join();
    numThreads++;
  }

  /*vector<point> points;
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
  }*/
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
