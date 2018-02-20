#include <fstream>
#include <string>
#include "reading.h"
#include "KDTree.h"
#include <iostream>
#include <cstdlib>

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
  KDTree * head = buildTree(points,n_cores);
  if (debug == 0)
  {
    printf("points in left %lu points in right %lu", head->left->allPoints.size(),head->right->allPoints.size());
  }
  //readQueries(argv[3],n_cores);
  //writeBinary(argv[4]);
}
