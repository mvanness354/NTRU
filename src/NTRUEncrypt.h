#include <NTL/ZZ_pX.h>
#include <NTL/ZZX.h>
#include <NTL/LLL.h>
#include <NTL/GF2X.h>
#include <random>
#include <vector>
#include <iostream>
#include <ctime>
#include <sstream>

using namespace std;
using namespace NTL;


/**
A Class for running the NTRU cryptosystem, with methods for key generation, encryption, and decryption.
Uses the NTL library for polynomial arithmetic, see the NTL documentation for more details.  
*/
class NTRUEncrypt {

	public:
		NTRUEncrypt();

		// Constructor with NTRU parameters N, p, q, d
		NTRUEncrypt(int N, int p, int q, int d);

		// Constructor with NTRU parameters N, p, q, d and random engine for controlling pseudo-randomness
		NTRUEncrypt(int N, int p, int q, int d, default_random_engine& rand);

		// Generates a ternary polynomial of length N with d1 1s and d2 -1s
		ZZX gen_ternary(int N, int d1, int d2);

		// Generates NTRU private keys f and g
		void gen_private_key();

		// Generates NTRU public key f*h mod q
		ZZ_pX get_public_key();

		// Encrypts a message m
		ZZ_pX encrypt(ZZX m);

		// Decrypts a ciphertext e
		ZZX decrypt(ZZ_pX& e);

		// Decrypts a ciphertext e using the given polynomial f_prime as the private key
		ZZX decrypt(ZZ_pX& e, ZZX& f_prime);

		// Decrypts a ciphertext e, using low and high and the boundaries for the lift
		ZZX decrypt(ZZ_pX& e, int low, int high);

		// Gets the width of the polynomial pgr + fm, meaning the max - min coefficients,
		// which is useful for finding Gap and Wrap failures
		ZZ get_width(ZZX& m);

	private:
		// NTRU params
		int N;
		int p;
		int q;
		int d;

		// NTRU private keys
		ZZX f;
		ZZX g;

		// Random polynomial r used during encryption
		ZZ_pX r;

		// Random engine
		default_random_engine rand_gen;

		// Polynomial to mod out by, normally x^N - 1
		ZZ_pX cyclo;

		// Set the NTL modulus
		void set_modulus(int p);

		// Compute the inverse of a polynomial when the modulus is 2^k.
		// needed because NTL cannot compute the inverse with a composite modulus
		ZZ_pX inverse2k(const ZZ_pX& f);

		// Reduce a polynomial from ZZX to ZZ_pX
		ZZ_pX reduce(const ZZX& f);

		// Change a polynomial from one modulus to another in ZZ_pX
		// by center lifting and then reducing
		ZZ_pX reduce(const ZZ_pX& f);

		// Center lift from ZZ_pX to ZZX
		ZZX center_lift(const ZZ_pX& f);

		// Lift from ZZ_pX to ZZX in the given range from low to high
		ZZX center_lift(const ZZ_pX& f, int low, int high);
};