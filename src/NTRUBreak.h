#include <NTL/ZZ_pX.h>
#include <NTL/ZZX.h>
#include <NTL/LLL.h>
#include <vector>
#include <sstream>

using namespace std;
using namespace NTL;


/** 
A Class for performing attacks against the NTRU cryptosystem.  Revolves around using
LLL-like lattice reduction techniques to recover the private key.  Uses the NTL library for
lattice reduction as well as polynomial arithmetic, see the NTL documentation for more details.
*/
class NTRUBreak {

	public:
		NTRUBreak();

		// Constructor that takes NTRU parameters N, p, q and public key h.
		NTRUBreak(int N, int p, int q, ZZ_pX& h);

		// Reduces the NTRU lattice using BKZ-LLL with the given block size.  The NTRU
		// matrix comes in the form [[I, h], [0, qI]].
		long reduce_lattice(int block_size);

		// Attempts to find rows in the NTRU matrix where, 
		// given the first N elements of the row as the polynomial f, 
		// f is ternary and f*h mod q is ternary.
		ZZX find_ternary(ZZ_pX& h);

		// Static method to check if two polynomials are cyclic rotations of each other.
		static bool is_rotation(ZZX& f, ZZX& g, int N);

		// Print the norm of each row of the NTRU matrix.
		void print_norms();

		// Print the smallest norm of the norms of the rows of the NTRU matrix.
		ZZ get_smallest_norm();


	private:
		mat_ZZ NTRU_lattice;
		int N;
		int p;
		int q;
		ZZ_pX cyclo; // the polynomial to mod out by, normally x^N - 1
		ZZ found_norm;

		// Create the NTRU matrix.
		void gen_NTRU_lattice(ZZ_pX& h);

		// Set the NTL modulus
		void set_modulus(int p);

		// Check if a polynomial is ternary
		bool is_ternary(const ZZX& f);

		// Center lift a polynomial from ZZ_pX to ZZX
		ZZX center_lift(const ZZ_pX& f);

		// Reduce a polynomial from ZZX to ZZ_pX
		ZZ_pX reduce(const ZZX& f);
};