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

class NTRUEncrypt {

	public:
		NTRUEncrypt();

		NTRUEncrypt(int N, int p, int q, int d);

		NTRUEncrypt(int N, int p, int q, int d, default_random_engine& rand);

		ZZX gen_ternary(int N, int d1, int d2);

		void gen_private_key();

		ZZ_pX get_public_key();

		ZZ_pX encrypt(ZZX m);

		ZZX decrypt(ZZ_pX& e);

		ZZX decrypt(ZZ_pX& e, ZZX& f_prime);

		ZZX decrypt(ZZ_pX& e, int low, int high);

		ZZX decrypt_times(ZZ_pX& e, ZZX& multiplier);

		ZZX get_a(ZZX& m);

		ZZX get_a(ZZX& m, ZZX& f_prime);

	private:
		int N;
		int p;
		int q;
		int d;

		ZZX f;
		ZZX g;

		ZZ_pX r;

		default_random_engine rand_gen;

		ZZ_pX cyclo;

		void set_modulus(int p);

		ZZ_pX inverse2k(const ZZ_pX& f);

		ZZ_pX reduce(const ZZX& f);

		ZZ_pX reduce(const ZZ_pX& f);

		ZZX center_lift(const ZZ_pX& f);

		ZZX center_lift(const ZZ_pX& f, int low, int high);
};