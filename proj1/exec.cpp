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

int main(int argc, char ** argv)
{
  if(argc!=5)
  {
    std::cerr<<"Insufficient arguments, enter the following \"num_cores input_file queries_file results_file\"";
  }
  //writeBinary("test");
  uint64_t n_cores;
  vector<point> points;
  n_cores = atoi(argv[1]);
  points = readInput(argv[2],n_cores);
  std::chrono::high_resolution_clock::time_point beforeBuild = std::chrono::high_resolution_clock::now();
  KDTree * head = buildTree(points,n_cores);
  std::chrono::high_resolution_clock::time_point afterBuild = std::chrono::high_resolution_clock::now(); 
  int i=0;
  while(head->right!=NULL)
  {
    head = head->right;
    i++;
  }

  if (debug == 0)
  {
    printf("points in head %lu, with depth %i", head->allPoints.size(),i);
  }
  //readQueries(argv[3],n_cores);
  //writeBinary(argv[4]);
}
