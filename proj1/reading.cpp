#include "reading.h"
using namespace std;

string inputFile;
string queryFile;
uint64_t inputID;
uint64_t queryID;
uint64_t numQueries;
uint64_t numDimensions;
uint64_t kNum;
uint64_t numIters;
//vector<point> knn;
const uint64_t ogOffset = 56;
uint64_t ID = 0;
bool debug = 0;
mutex vectorMutex;
static KDTree * head;

vector<point> readInput(string filename)
{
  //std::chrono::high_resolution_clock::time_point beforeBuild = std::chrono::high_resolution_clock::now();
  inputFile = filename;
  ifstream file (inputFile, ios::in|ios::binary);
  char type[8];
  file.read(reinterpret_cast<char *>(&type),sizeof(type));
  file.read(reinterpret_cast<char *>(&inputID),sizeof(inputID));
  uint64_t numEntries;
  file.read(reinterpret_cast<char *>(&numEntries),sizeof(numEntries));
  uint64_t numDim;
  file.read(reinterpret_cast<char *>(&numDim),sizeof(numDim));
  numDimensions=numDim;
  printf("Header: %s %lu %lu %lu\n",type,inputID,numEntries,numDimensions);

  //uint32_t test;
  //file.read(reinterpret_cast<char *>(&test),sizeof(&test));
  //printf("first: %u\n",test);
  vector<point> points;
  uint64_t j = 0;
  float val;
  while(!file.eof())
  {
    float vals[numDimensions];
    j=0;
    while(j<numDimensions)
      {
	file.read(reinterpret_cast<char *>(&val),sizeof(float));
	vals[j]=val;
	j++;
      }

    if(file.ios::fail())
    {
      //cerr<<"wut";
      break;
    }


    points.push_back(point(numDimensions,vals)); //IS THIS ALLOWED?!??!?!?!!?




    //printf("Point: %f %f\n",vals[0],vals[1]);
  }
  //std::chrono::high_resolution_clock::time_point afterBuild = std::chrono::high_resolution_clock::now();
  //std::chrono::duration<double> timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(afterBuild-beforeBuild);
  //cerr<<"it took " << timeElapsed.count() << " seconds read the tree";
  if(points.size()!=numEntries)
    {
      cerr<<"something happened reading in this stuff";
    }
  if(debug == 0){
    printf("Header repeat: %lu %f\n",points.size(),points[0].values[1]);
  }
  return points;
  //printf("Point: %u %u\n",points[0].values[0],points[0].values[1]);
  //delete &points;
}

void readPoints(uint64_t offset,uint64_t numPoints,string filename,string rfilename,uint64_t threadNum, uint64_t numCores)//,vector<point> allPoints)//in case we want to test our brtueforce in comparison
{
  uint64_t i = 0;
  uint64_t j=0;
  uint64_t dC = 0;
  uint64_t counter=0;
  float val;
  ifstream file (filename, ios::in|ios::binary);
  uint64_t resultFileLoc;
  ofstream fileR;
  if(threadNum==0)
    {
      resultFileLoc = ogOffset;
    }
  else
    {
      resultFileLoc = ogOffset+(numQueries/numCores)*threadNum*(kNum);
      //printf("\nHELLO%lu\n",resultFileLoc);
    }
  char name[8] = "RESULT";

  fileR.open(rfilename, ios::out|ios::binary);
  if(threadNum == 0)
    {
      //cerr<<"WEWRITINGINHERE";
      //file.write((char *) &type,sizeof(type));
      fileR.write((char *) &name,sizeof(name));
      fileR.write((char *) &inputID,sizeof(inputID));
      fileR.write((char *) &queryID,sizeof(queryID));
      fileR.write((char *) &ID,sizeof(ID));
      fileR.write((char *) &numQueries,sizeof(numQueries));
      fileR.write((char *) &numDimensions,sizeof(numDimensions));
      fileR.write((char *) &kNum,sizeof(kNum));
    }
  else
    {
       fileR.seekp(resultFileLoc);
    }
  //cerr<<offset<<endl;
  file.seekg(offset);

  //fileR.close();
  while(i<numPoints)
  {

    float vals[numDimensions];
    j=0;
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
    point p = point(numDimensions,vals);
    auto query = make_unique<point>(p);
    vector<point> knn = getKNearestNeighbors(*query.get());

    /*printf("%f,%f with NN \n",query.values[0],query.values[1]);*/

    counter=0;
    while(counter<kNum)
      {
	dC=0;
	//printf("\nPoint: ");
	while(dC<numDimensions)
	  {
	    //printf("%f, ",knn[counter].values[dC]);
	    fileR.write((char *) &knn[counter].values[dC],sizeof(knn[counter].values[dC]));
	    dC++;
	  }
	counter++;
      }

    //testKNN(allPoints,query,kNum);//only if we want to test our brute force in comparison
    j=0;
    ++i;
  }
  fileR.close();
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
    cerr<<sumDist<<endl;
  }
  if(sqrt(sumDist)<=0)
  {
    cerr<<"whut";
  }
  //cerr<<sumDist;
  return (float)sqrt(sumDist);
}

vector<point> getKNearestNeighbors(point &query)
{
  KDTree * root = head;
  uint64_t DS=0;
  vector<point> og;
  return recursiveKNN(root,DS,og,query);

}

vector< pair<float,int>> getMinPoints(vector<point> og,point query)
{
  uint64_t i = 0;
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
    newCurrPoints = recursiveKNN(node->right.get(),(DS+1)%numDimensions,currPoints,query);
    if(newCurrPoints.size()<kNum && node->left!=NULL)//we havent found enough neighbors yet! go down the other branch
    {
      newCurrPoints = recursiveKNN(node->left.get(),(DS+1)%numDimensions,newCurrPoints,query);
    }
    else // check difference between splitDimension points, we have enough neighboors, but maybe there is more to go
    {
      if(query.values[DS] - node->allPoints[0].values[DS] < calcDist(query,newCurrPoints[kNum-1]))//distance means there could be a point closer on the other side!
      {
        if(node->left!=NULL)
        {
          newCurrPoints = recursiveKNN(node->left.get(),(DS+1)%numDimensions,newCurrPoints,query);
        }
      }
    }
    if(calcDist(query,node->allPoints[0]) < calcDist(query,newCurrPoints[kNum-1])) //see if the point here is any good
    {
      uint64_t i = 0;
      while(i<newCurrPoints.size())
      {
        if(calcDist(query,node->allPoints[0]) < calcDist(query,newCurrPoints[i]))
        {
          newCurrPoints.insert(newCurrPoints.begin()+i,move(node->allPoints[0]));
          break;
        }
        ++i;
      }
      newCurrPoints.pop_back();
    }
  }
  else if(node->allPoints[0].values[DS]>query.values[DS] && node->left !=NULL)
  {
    newCurrPoints = recursiveKNN(node->left.get(),(DS+1)%numDimensions,currPoints,query);
    if(newCurrPoints.size()<kNum && node->right!=NULL)
    {
      newCurrPoints = recursiveKNN(node->right.get(),(DS+1)%numDimensions,newCurrPoints,query);
    }
    else // check difference between splitDimension points
    {
      if(query.values[DS] - node->allPoints[0].values[DS] < calcDist(query,newCurrPoints[kNum-1]))//distance means there could be a point closer on the other side!
      {
        if(node->right!=NULL)
        {
          newCurrPoints = recursiveKNN(node->right.get(),(DS+1)%numDimensions,newCurrPoints,query);
        }
      }
    }
    if(calcDist(query,node->allPoints[0]) < calcDist(query,newCurrPoints[kNum-1]))
    {
      uint64_t i = 0;
      while(i<newCurrPoints.size())
      {
        if(calcDist(query,node->allPoints[0]) < calcDist(query,newCurrPoints[i]))
        {
          newCurrPoints.insert(newCurrPoints.begin()+i,move(node->allPoints[0]));
          break;
        }
        ++i;
      }
      newCurrPoints.pop_back();
    }
  }
  else if(node->left==NULL && node->right == NULL)
  {
    uint64_t i =0;
    int sz= node->allPoints.size();
    int sz2;
    vector< pair<float,int>> minPoints = getMinPoints(node->allPoints,query);
    if(currPoints.size()==0)
    {
      while(currPoints.size()<=kNum && i < node->allPoints.size())
      {
        //cerr<<"beg"<<minPoints[i].second<<"end"<<endl;
        newCurrPoints.push_back(move(node->allPoints[minPoints[i].second]));
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
          newCurrPoints.push_back(move(node->allPoints[minPoints[x].second]));
          ++x;
        }
        else
        {
          newCurrPoints.push_back(move(currPoints[y]));
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
          newCurrPoints.push_back(move(currPoints[y]));
          ++y;
        }
      }
      else if(y==sz2 && x<sz)
      {
        while(x<sz && newCurrPoints.size()<kNum)
        {
          newCurrPoints.push_back(move(node->allPoints[minPoints[x].second]));
          ++x;
        }
      }
    }

  }
  return newCurrPoints;

}
void readQueries(string filename, uint64_t numCores,KDTree * root,string rname,vector<point> allPoints)
{
  //below code on getting random value was taken from https://stackoverflow.com/questions/35726331/c-extracting-random-numbers-from-dev-urandom
  //Declare value to store data into
  size_t size = sizeof(ID); //Declare size of data
  ifstream urandom("/dev/urandom", ios::in|ios::binary); //Open stream
  if(urandom) //Check if stream is open
    {
      urandom.read(reinterpret_cast<char*>(&ID), size); //Read from urandom
      if(urandom) //Check if stream is ok, read succeeded
        {
	  std::cout << "Read random value: " << ID << std::endl;
        }
      else //Read failed
        {
	  ID=0;
	  std::cerr << "Failed to read from /dev/urandom" << std::endl;
        }
      urandom.close(); //close stream
    }
  else //Open failed
    {
      ID = 0;
      std::cerr << "Failed to open /dev/urandom" << std::endl;
    }

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
  numIters = numQueries/numCores;//how many we are going to do
  uint64_t offsetData = numIters*querySize;//4 is the size for each 32 bit in a points value, times the number of dimensions
  thread readers[numCores];
  uint64_t numThreads = 0;
  if(debug == 0){
    printf("Using %lu threads, we assign each thread %lu points, leaving the last thread with %lu entries.\n",numCores,numIters, numQueries-numIters*(numCores-1));
  }
  while(numThreads<numCores-1)//leave remaining points to last thread
  {
    readers[numThreads]= thread(readPoints,inputFileoffset+numThreads*offsetData,numIters,filename,rname,numThreads,numCores);
    numThreads++;
  }
  readers[numThreads]=thread(readPoints,inputFileoffset+numThreads*offsetData,numQueries-numIters*numThreads,filename,rname,numThreads,numCores);//these are the remaining points

  numThreads=0;
  while (numThreads<numCores)
  {
    readers[numThreads].join();
    numThreads++;
  }
  //writeBinary("yuh");
  readResults(rname);
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

void writeBinary(string filename)
{
  ofstream file;
  char type[8] = "RESULT";
  uint64_t numEntries = 100;
  uint64_t numDim = 4;
  uint32_t x1 = 1;
  uint32_t y1 = 2;
  uint32_t z1 = 3;
  uint32_t w1 = 4;

  file.open(filename,ios::out|ios::binary);
  file.write((char *) &type,sizeof(type));
  file.write((char *) &numEntries,sizeof(numEntries));
  file.write((char *) &numDim,sizeof(numDim));
  file.write((char *) &x1,sizeof(x1));
  file.write((char *) &y1,sizeof(y1));
  file.write((char *) &z1,sizeof(z1));
  file.write((char *) &w1,sizeof(w1));
}

void readResults(string rfileName)
{
  //cerr<<"wut";
  ifstream file (rfileName, ios::in|ios::binary);
  char type[8]; // may want to generalize this;
  uint64_t inptID;
  uint64_t querID;
  uint64_t resultID;
  uint64_t nQ;
  uint64_t nD;
  uint64_t knn;
  file.read(reinterpret_cast<char *>(&type),sizeof(type));
  type[7]='\0';
  file.read(reinterpret_cast<char *>(&inptID),sizeof(inptID));
  file.read(reinterpret_cast<char *>(&querID),sizeof(querID));
  file.read(reinterpret_cast<char *>(&resultID),sizeof(resultID));
  file.read(reinterpret_cast<char *>(&nQ),sizeof(nQ));
  file.read(reinterpret_cast<char *>(&nD),sizeof(nD));
  file.read(reinterpret_cast<char *>(&knn),sizeof(knn));
  printf("\nHeader: %s %lu %lu %lu %lu %lu %lu\n",type,inptID, querID,resultID, nQ,nD,knn);
  float x1;
  float y1;
  int i = 0;
  if(debug==1)
    {
      while(i<10)
	     {
    	   file.read(reinterpret_cast<char *>(&x1),sizeof(x1));
	       file.read(reinterpret_cast<char *>(&y1),sizeof(y1));
	       ++i;
	       printf("\n%f,%f\n",x1,y1);
	     }
    }

}
