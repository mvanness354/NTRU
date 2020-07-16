#include <NTL/ZZ_pX.h>
#include <NTL/ZZX.h>
#include <NTL/LLL.h>
#include <vector>
#include <sstream>

using namespace std;
using namespace NTL;

class NTRUBreak {

	public:
		NTRUBreak();
		NTRUBreak(int N, int p, int q, ZZ_pX& h);

		long reduce_lattice(int block_size);

		ZZX find_ternary(ZZ_pX& h);

		static bool is_rotation(ZZX& f, ZZX& g, int N);

		void print_norms();

		ZZ get_smallest_norm();


	private:
		mat_ZZ NTRU_lattice;

		void gen_NTRU_lattice(ZZ_pX& h);

		void set_modulus(int p);

		bool is_ternary(const ZZX& f);

		ZZX center_lift(const ZZ_pX& f);

		ZZ_pX reduce(const ZZX& f);

		int N;
		int p;
		int q;

		ZZ_pX cyclo;

		ZZ found_norm;

};