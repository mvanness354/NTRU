g++ -g -O2 -std=c++11 -pthread -march=native src/NTRUEncrypt.cpp src/NTRUBReak.cpp $1 -o $1.o -lntl -lgmp -lm
