INPUT=$1
FILE="${INPUT##*/}"
FILE="${FILE%%.*}"
g++ -g -O2 -std=c++11 -pthread -march=native src/NTRUEncrypt.cpp src/NTRUBReak.cpp $1 -o $FILE.o -lntl -lgmp -lm