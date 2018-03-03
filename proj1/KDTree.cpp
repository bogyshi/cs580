#include "KDTree.h"
#include "reading.h"
#include "point.h"
#include <assert.h>
#include <thread>
#include <limits.h>
//CXX= -Wextra/...
using namespace std;
random_device rd;
mt19937 gen(rd());
static uniform_int_distribution<uint64_t> dist(0,ULLONG_MAX);
static const uint64_t THRESH = 10;
static uint64_t availableThreads; //locked by mutex mx
static uint64_t workAvailable; //locked by mutex mx
static queue< pair<KDTree *, vector<point>>> workingQueue; // locked by mutex mx
static uint64_t numCores;
static uint64_t maxThreads;
static const debug;
static const float samplePercent = 0.10;
mutex mx;
//shared_mutex numThreads;
condition_variable workToDo;
KDTree::KDTree()
{
  splitDim=0;
  //numPoints=0;
  left = NULL;
  right = NULL;
}
KDTree::KDTree(uint64_t dimSplit)
{
  splitDim=dimSplit;
  //numPoints=0;
  left = NULL;
  right = NULL;
}
unique_ptr<KDTree> buildTree(vector<point> points,uint64_t nCores)
{
  unique_ptr<KDTree> head = make_unique<KDTree>();
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
      leftPoints.push_back(move(points[i]));
    }
    else if(points[i].values[0]>val)
    {
      rightPoints.push_back(move(points[i]));
    }
    else
    {
      cerr<<"!!!";
      head->allPoints.push_back(move(points[i]));
      //head->numPoints++;
    }
    ++i;
  }
  availableThreads = numCores;
  maxThreads = availableThreads;

  head->left = make_unique<KDTree>(1);
  head->right = make_unique<KDTree>(1);
  /*head->depth = 0;
  head->left->depth=1;
  head->right->depth=1;*/ //uncomment to bring back depth
  workingQueue.push(pair<KDTree *, vector<point>>(head->left.get(),leftPoints));
  workingQueue.push(pair<KDTree *, vector<point>>(head->right.get(),rightPoints));
  workAvailable=2;
  i = 0;
  thread builders[numCores];
  while(i<maxThreads)
  {
    builders[i] = thread(completeTree,points[0].values.size());
    ++i;
  }

  while(true)
  {
    //unique_lock<mutex> lck(mx);
    //workToDo.wait(lck);
    //cerr<<"stuck";
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
      workToDo.notify_one();
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
  if(debug==1)
  {
    struct mallinfo mi = mallinfo();
    //the below was stolen from the manpage on mallinfo
    printf("Total non-mmapped bytes (arena):       %d\n", mi.arena);
    printf("# of free chunks (ordblks):            %d\n", mi.ordblks);
    printf("# of free fastbin blocks (smblks):     %d\n", mi.smblks);
    printf("# of mapped regions (hblks):           %d\n", mi.hblks);
    printf("Bytes in mapped regions (hblkhd):      %d\n", mi.hblkhd);
    printf("Max. total allocated space (usmblks):  %d\n", mi.usmblks);
    printf("Free bytes held in fastbins (fsmblks): %d\n", mi.fsmblks);
    printf("Total allocated space (uordblks):      %d\n", mi.uordblks);
    printf("Total free space (fordblks):           %d\n", mi.fordblks);
    printf("Topmost releasable block (keepcost):   %d\n", mi.keepcost);
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
    //cerr<<"doing stuff?";
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
      workToDo.notify_one();
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
        if(points[i].values[head->splitDim]<val)
        {
          leftPoints.push_back(move(points[i]));
        }
        else if(points[i].values[head->splitDim]>val)
        {
          rightPoints.push_back(move(points[i]));
        }
        else
        {
          head->allPoints.push_back(move(points[i]));
          //head->numPoints++;
        }
        ++i;
      }

      lck.lock(); //MAY WANT TO MOVE THIS
      if(leftPoints.size()>0)
      {
        head->left = make_unique<KDTree>(((head->splitDim)+1)%numDim);
        //head->left->depth = head->depth+1;
        workingQueue.push(pair<KDTree *, vector<point>>(head->left.get(),leftPoints));
        workAvailable+=1;
      }
      if(rightPoints.size()>0)
      {
        head->right = make_unique<KDTree>(((head->splitDim)+1)%numDim);
        //head->right->depth = head->depth+1;
        workingQueue.push(pair<KDTree *, vector<point>>(head->right.get(),rightPoints));
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
  uint64_t i=0;
  uint64_t sz = points.size();
  uint64_t loc;
  point val;
  vector<float> holder;
  if(sz<100)//points.size()<100) // so small we will determine a true median
  {
    while(i<sz)
    {
      val = points[i];
      holder.push_back(val.values[DTS]);
      ++i;
    }
    nth_element(holder.begin(),holder.begin()+holder.size()/2,holder.end());//found here http://en.cppreference.com/w/cpp/algorithm/nth_element

    //nth_element(points.begin(),points.begin()+points.size()/2,points.end());
  }
  else
  {
    uint64_t numSam = sz*samplePercent;
    while(i<numSam)
    {
      loc = dist(gen)%sz;
      val = points[loc];
      holder.push_back(val.values[DTS]);
      ++i;
    }
    nth_element(holder.begin(),holder.begin()+holder.size()/2,holder.end());//found here http://en.cppreference.com/w/cpp/algorithm/nth_element
  }
  return holder[holder.size()/2];
}

void testKNN(std::vector<point> allPoints, point query, uint64_t kNN)
{
  uint64_t sz = allPoints.size();
  uint64_t counter = 0;
  uint64_t tempCounter;
  vector<point> minPoints;
  minPoints.push_back(allPoints[counter]);
  counter++;
  while(counter<sz)
    {
      if(minPoints.size()<kNN)
	{
	  tempCounter=0;
	  while(tempCounter<minPoints.size())
	    {
	      if(calcDist(query,allPoints[counter])<calcDist(query,minPoints[tempCounter]))
		{
		  minPoints.insert(minPoints.begin()+tempCounter,allPoints[counter]);
		  break;
		}
	      tempCounter++;
	    }
	}
      else if(calcDist(query,allPoints[counter])<calcDist(query,minPoints[minPoints.size()-1]))//it is a lesser weight than what we already have
	{
	  tempCounter=0;
	  while(tempCounter<minPoints.size())
	    {
	      if(calcDist(query,allPoints[counter])<calcDist(query,minPoints[tempCounter]))
		{
		  minPoints.insert(minPoints.begin()+tempCounter,allPoints[counter]);
		  minPoints.pop_back();
		  break;
		}
	      tempCounter++;
	    }
	}
      counter++;
    }
  counter = 0;
  printf("\nBEGIN BRUTEFORCECALC\n");
  while(counter<minPoints.size())
    {
      printf("%f,%f with dist %f\n",minPoints[counter].values[0],minPoints[counter].values[1],calcDist(query,minPoints[counter]));
      counter++;
    }
}
