#include <fstream>
#include <string>
#include "reading.h"
#include "KDTree.h"
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <ratio>

using namespace std;

const bool debug = 0;
uint64_t avgDepth(KDTree * head);
pair<uint64_t, uint64_t> testBalance(KDTree * node);

int main(int argc, char ** argv)
{
  if(argc!=5)
  {
    std::cerr<<"Insufficient arguments, enter the following \"num_cores input_file queries_file results_file\"";
  }
  //writeBinary("test");
  uint64_t n_cores;
  vector<point>  points;
  n_cores = atoi(argv[1]);
  //std::chrono::high_resolution_clock::time_point beforeRead = std::chrono::high_resolution_clock::now();
  points = readInput(argv[2]);
  //std::chrono::high_resolution_clock::time_point afterRead = std::chrono::high_resolution_clock::now();
  //std::chrono::duration<double> timeRead = std::chrono::duration_cast<std::chrono::duration<double>>(afterRead-beforeRead);
  //printf("it took %f seconds to read the tree",timeRead.count());
  std::chrono::high_resolution_clock::time_point beforeBuild = std::chrono::high_resolution_clock::now();
  unique_ptr<KDTree> head = buildTree(points,n_cores);
  std::chrono::high_resolution_clock::time_point afterBuild = std::chrono::high_resolution_clock::now();
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
  std::chrono::duration<double> timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(afterBuild-beforeBuild);

  cerr<<"it took " << timeElapsed.count() << " seconds to build the tree\n";
  if (debug == 1)
  {
    //printf("points in head %lu, with depth %i\n", head->allPoints.size(),i);
    printf("avg depth %lu\n", avgDepth(head.get()));
  }
  vector<point> pointless;//place holder for as long as we arent debugging
  beforeBuild = std::chrono::high_resolution_clock::now();
  readQueries(argv[3],n_cores,head.get(),argv[4],pointless);
  afterBuild = std::chrono::high_resolution_clock::now();
  timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(afterBuild-beforeBuild);
  cerr<<"it took " << timeElapsed.count() << " seconds to solve queries\n";

  //writeBinary(argv[4]);
}

uint64_t avgDepth(KDTree * head)
{
  pair<uint64_t, uint64_t > result = testBalance(head);
  return(result.first/result.second);
}

pair<uint64_t, uint64_t> testBalance(KDTree * node)
{
  /*
  if(node->left==NULL && node->right==NULL)
  {
    return pair<uint64_t, uint64_t >(node->depth,1);
  }
  else if(node->left!=NULL && node->right!=NULL)
  {
    pair<uint64_t, uint64_t > temp1 = testBalance(node->left.get());
    pair<uint64_t, uint64_t > temp2 = testBalance(node->right.get());
    pair<uint64_t, uint64_t > result = pair<uint64_t, uint64_t >(temp1.first+temp2.first,temp1.second+temp2.second);
    return result;
  }
  else if(node->left!=NULL)
  {
    return testBalance(node->left.get());
  }
  else
  {
    return testBalance(node->right.get());
  }
  */
  //return NULL;
}
