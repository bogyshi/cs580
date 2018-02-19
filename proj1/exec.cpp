#include <fstream>
#include <string>
#include "reading.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char ** argv)
{
  if(argc!=5)
  {
    std::cerr<<"Insufficient arguments, enter the following \"num_cores input_file queries_file results_file\"";
  }
  //writeBinary("test");
  uint64_t n_cores;
  n_cores = atoi(argv[1]);
  readInput(argv[2],n_cores);
  readQueries(argv[3]);
  writeResults(argv[4]);
}
