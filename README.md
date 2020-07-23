# NTRU
The following is a straight-forward implementation of the NTRU cryptosystem in C++.  NTRU is a lattice-based cryptosystem which has promising qualities as a 
post-quantum replacement to RSA.  For more details on NTRU, see [wikipedia](https://en.wikipedia.org/wiki/NTRUEncrypt) as well as the original NTRU paper,
[NTRU: A Ring Based Public Key Cryptosystem](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.25.8422&rep=rep1&type=pdf).  Another fantastic resource is
the textbook *An Introduction to Mathematical Cryptography*, particularly Chapter 7.  

# Dependencies
This implementation requires the installation of [NTL](https://www.shoup.net/ntl/), an extensive number theory library.  Please refer to the NTL documentation for
details on how to install NTL.

# Compiling and Running
A compile script, `ntru_compile.sh`, is provided to assist with compilation.  This script uses the C++ compiler g++ and the appropriate flags for compilation.
Simply supply the script with the main file which you are using, for example, `./ntru_compile.sh example_main.cpp`.  It is also acceptable to use a different
C++ compiler, just make sure to compile all of the files in the `src` directory.
