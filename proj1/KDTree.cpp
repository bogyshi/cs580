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
KDTree * buildTree(vector<point> points,uint64_t numCores)
{
  KDTree * head = new KDTree();
  uint32_t val = sampledMedian(points,0);
  uint64_t i = 0;
  uint64_t sz = points.size();
  vector<point> leftPoints;
  vector<point> rightPoints;
  queue< pair<KDTree *, vector<point>>> workingQueue;
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
  head->left = new KDTree();
  head->right = new KDTree();
  workingQueue.push(pair<KDTree *, vector<point>>(head->left,leftPoints));
  workingQueue.push(pair<KDTree *, vector<point>>(head->right,rightPoints));
  thread builders[numCores];
  return head;
}

uint32_t sampledMedian(vector<point> points, uint64_t DTS)//DTS=dimensionToSplit
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
    vector<uint32_t> holder;
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
