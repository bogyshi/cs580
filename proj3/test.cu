#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/sequence.h>
#include <thrust/copy.h>
#include <thrust/fill.h>
#include <thrust/replace.h>
#include <thrust/functional.h>
#include <cmath>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include <iostream>
#include <iomanip>

using namespace std;

void swap(int &i) {
    // Some of the & are superfluous.
    i =
     (0xff&(i >> 24)) |
     (0xff00&(i >> 8)) |
     (0xff0000&(i << 8)) |
     (0xff000000&(i << 24));
}

int read_int(int fd) {
    int rv;
    int i;
    rv = read(fd, &i, 4); assert(rv == 4);
    swap(i);
    return i;
}

void
output_pgm(const std::string &fn, const float (&img)[28][28]) {

    std::ofstream ofs(fn, std::fstream::out|std::fstream::trunc);

    ofs << "P2\n";
    ofs << "28 28\n";
    ofs << "255\n";
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            if (j > 0) {
                ofs << " ";
            }
            ofs << 255 - int(std::round(127.5*(img[i][j] + 1)));
        }
        ofs << "\n";
    }
}


template <int N>
void
read_mnist_images(const std::string &fn, float (&imgs)[N][28][28]) {

    int rv;

    int fd;
    fd = open(fn.c_str(), O_RDONLY);
    assert(fd >= 0);

    int magic = read_int(fd);
    assert(magic == 0x803);

    int n_images = read_int(fd);
    assert(n_images == N);

    int n_rows = read_int(fd);
    assert(n_rows == 28);

    int n_cols = read_int(fd);
    assert(n_cols == 28);

    for (int i = 0; i < N; i++) {
        unsigned char tmp[28][28];
        rv = read(fd, tmp, 28*28); assert(rv == 28*28);
        for (int r = 0; r < 28; r++) {
            for (int c = 0; c < 28; c++) {
                // Make go from -1 to 1.
                imgs[i][r][c] = double(tmp[r][c])/127.5 - 1;
            }
        }
    }

    rv = close(fd); assert(rv == 0);
}

template <int N>
void
read_mnist_labels(const std::string &fn, unsigned char (&labels)[N]) {

    int rv;

    int fd;
    fd = open(fn.c_str(), O_RDONLY);
    assert(fd >= 0);

    int magic = read_int(fd);
    assert(magic == 0x801);

    int n_labels = read_int(fd);
    assert(n_labels == N);

    rv = read(fd, labels, N); assert(rv == N);
    for (int i = 0; i < N; i++) {
        assert(labels[i] >= 0 && labels[i] <= 9);
    }

    rv = close(fd); assert(rv == 0);
}

int main(void)
{
    // allocate three device_vectors with 10 elements
    string imagename = "/data/home/avanroi1/cs580f/cs580/proj3/training/train-images-idx3-ubyte";
    string labelname = "/data/home/avanroi1/cs580f/cs580/proj3/training/train-labels-idx1-ubyte";

    static float training_images[60'000][28][28];
    read_mnist_images(imagename, training_images);
    //output_pgm("img0.pgm", training_images[0]);
    //output_pgm("img59999.pgm", training_images[59999]);

    static unsigned char training_labels[60'000];
    read_mnist_labels(labelname, training_labels);
    assert(training_labels[0] == 5);
    assert(training_labels[59'999] == 8);

    thrust::device_vector<int> X(10);
    thrust::device_vector<int> Y(10);
    thrust::device_vector<int> Z(10);
    //just tryna scrap
    // initialize X to 0,1,2,3, ....
    thrust::sequence(X.begin(), X.end());

    // compute Y = -X
    thrust::transform(X.begin(), X.end(), Y.begin(), thrust::negate<int>());
    thrust::copy(Y.begin(), Y.end(), std::ostream_iterator<int>(std::cout, "\n"));

    // fill Z with twos
    thrust::fill(Z.begin(), Z.end(), 2);

    // compute Y = X mod 2
    thrust::transform(X.begin(), X.end(), Z.begin(), Y.begin(), thrust::modulus<int>());

    // replace all the ones in Y with tens
    thrust::replace(Y.begin(), Y.end(), 1, 10);

    // print Y
    thrust::copy(Y.begin(), Y.end(), std::ostream_iterator<int>(std::cout, "\n"));

   
    return 0;    
}