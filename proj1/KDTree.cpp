#include "KDTree.h"
#include "reading.h"
#include "point.h"
#include <assert.h>
#include <thread>
#include <limits.h>

using namespace std;
random_device rd;
mt19937 gen(rd());
static uniform_int_distribution<uint64_t> dist(0,ULLONG_MAX);
static const uint64_t THRESH = 10;
static uint64_t availableThreads;
static uint64_t workAvailable;
static queue< pair<KDTree *, vector<point>>> workingQueue;
static uint64_t numCores;
static uint64_t maxThreads;
mutex mx;
//shared_mutex numThreads;
condition_variable workToDo;
KDTree::KDTree()
{
  splitDim=0;
  numPoints=0;
  left = NULL;
  right = NULL;
}
KDTree::KDTree(uint64_t dimSplit)
{
  splitDim=dimSplit;
  numPoints=0;
  left = NULL;
  right = NULL;
}

KDTree * buildTree(vector<point> points,uint64_t nCores)
{
  KDTree * head = new KDTree();
  float val = sampledMedian(points,0);
  uint64_t i = 0;
  uint64_t sz = points.size();
  vector<point> leftPoints;
  vector<point> rightPoints;
  numCores = nCores;

  while(i<sz)
  {
    if(points[i].values[0]<val)
    {
      leftPoints.push_back(points[i]);
    }
    else if(points[i].values[0]>val)
    {
      rightPoints.push_back(points[i]);
    }
    else
    {
      head->allPoints.push_back(points[i]);
      head->numPoints++;
    }
    ++i;
  }
  availableThreads = numCores;
  maxThreads = availableThreads;

  head->left = new KDTree(1);
  head->right = new KDTree(1);
  workingQueue.push(pair<KDTree *, vector<point>>(head->left,leftPoints));
  workingQueue.push(pair<KDTree *, vector<point>>(head->right,rightPoints));
  workAvailable=2;
  i = 0;
  thread builders[numCores];
  while(i<maxThreads)
  {
    builders[i] = thread(completeTree,points[0].numDim);
    ++i;
  }

  while(true)
  {
    //unique_lock<mutex> lck(mx);
    //workToDo.wait(lck);
    cerr<<"stuck";
    if(availableThreads==maxThreads && workAvailable==0)
    {
      cerr<<"work is officially done"<<endl;
      break;
    }
  }

  //all threads are done doing stuff and nothing is on the queue now
  if(workAvailable == 0)
  {
    i=0;
    while(i<maxThreads)
    {
      cerr<<"waiting for thread"<<i<<endl;
      workToDo.notify_all();
      ++i;
    }
    i=0;
    while(i<maxThreads)
    {
      cerr<<"waiting for thread"<<i<<endl;
      builders[i].join();//need a way to end all threads when they are done
      ++i;
    }
  }
  else
  {
    printf("ERROR: all threads done working but no work available?!");
  }
  return head;
}

void completeTree(uint64_t numDim)
{
  uint64_t i;
  uint64_t sz;
  float val;
  while(true)
  {
    //check if all threads are available and break if that is true

    unique_lock<mutex> lck(mx);
    while(workAvailable<=0)
    {
      workToDo.wait(lck);
      if(workAvailable==0 && availableThreads==maxThreads)
      {
        break;
      }
      //cerr<<"waiting";
    }
    if(workAvailable==0 && availableThreads==maxThreads)
    {
      lck.unlock();
      break;
    }
    pair<KDTree *, vector<point>> temp = workingQueue.front();
    workingQueue.pop();
    workAvailable--;
    availableThreads--;
    lck.unlock();

    vector<point> points = temp.second;
    KDTree * head = temp.first;
    if(points.size()>5)
    {
      val = sampledMedian(points,head->splitDim);
      i = 0;
      sz = points.size();
      vector<point> leftPoints;
      vector<point> rightPoints;
      while(i<sz)
      {
        if(points[i].values[0]<val)
        {
          leftPoints.push_back(points[i]);
        }
        else if(points[i].values[0]>val)
        {
          rightPoints.push_back(points[i]);
        }
        else
        {
          head->allPoints.push_back(points[i]);
          head->numPoints++;
        }
        ++i;
      }

      lck.lock(); //MAY WANT TO MOVE THIS
      if(leftPoints.size()>0)
      {
        head->left = new KDTree(((head->splitDim)+1)%numDim);
        workingQueue.push(pair<KDTree *, vector<point>>(head->left,leftPoints));
        workAvailable+=1;
      }
      if(rightPoints.size()>0)
      {
        head->right = new KDTree(((head->splitDim)+1)%numDim);
        workingQueue.push(pair<KDTree *, vector<point>>(head->right,rightPoints));
        workAvailable+=1;
      }

      availableThreads++;
      workToDo.notify_all();
      lck.unlock();
    }
    else
    {
      //cerr<<points[0].values[0];
      head->allPoints = points;
      lck.lock();
      availableThreads++;
      lck.unlock();
    }

  }

}

float sampledMedian(vector<point> points, uint64_t DTS)//DTS=dimensionToSplit
{
  if(false)//points.size()<100) // so small we will determine a true median
  {
    //nth_element(points.begin(),points.begin()+points.size()/2,points.end());
  }
  else
  {
    uint64_t i=0;
    uint64_t sz = points.size();
    uint64_t loc;
    point val;
    vector<float> holder;
    while(i<sz*0.05)
    {
      loc = dist(gen)%sz;
      val = points[loc];
      holder.push_back(val.values[DTS]);
      ++i;
    }
    nth_element(holder.begin(),holder.begin()+holder.size()/2,holder.end());//found here http://en.cppreference.com/w/cpp/algorithm/nth_element
    return holder[holder.size()/2];
  }
}
