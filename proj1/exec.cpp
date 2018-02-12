#include <fstream>
#include <string>
#include "reading.h"
#include <iostream>

using namespace std;

int main(int argc, char ** argv)
{
  if(argc!=5)
  {
    std::cerr<<"Insufficient arguments, enter the following \"num_cores input_file queries_file results_file\"";
  }
  writeBinary("test");
  readInput(argv[2]);
}
