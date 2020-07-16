#include "NTRUBreak.h"

using namespace std;
using namespace NTL;

NTRUBreak::NTRUBreak() {}

NTRUBreak::NTRUBreak(int N, int p, int q, ZZ_pX& h) {
	this->N = N;
	this->p = p;
	this->q = q;

	gen_NTRU_lattice(h);
}

void NTRUBreak::set_modulus(int p) {
	ZZ_p::init(ZZ(p));
	cyclo = ZZ_pX(INIT_MONO, N) - 1;
}

ZZX NTRUBreak::center_lift(const ZZ_pX& f) {
	ZZ prod = ZZ(1);
	ZZX f_lift;
	CRT(f_lift, prod, f);
	return f_lift;
}

ZZ_pX NTRUBreak::reduce(const ZZX& f) {
	ZZ_pX f_modp;
	stringstream buffer;
	buffer << f;
 	buffer >> f_modp;
 	return f_modp;
}

bool NTRUBreak::is_ternary(const ZZX& f) {
	if (deg(f) == -1) return false;

	for (int i=0; i<f.rep.length(); i++) {
		if (f[i] != -1 && f[i] != 0 && f[i] != 1) {
			return false;
		}
	}
	return true;
}

void NTRUBreak::gen_NTRU_lattice(ZZ_pX& h) {
	NTRU_lattice.SetDims(2*N, 2*N);

	vector<ZZ> h_coeffs(2*N);
	for (int i=0; i<N; i++) {
		h_coeffs[i] = (i <= deg(h)) ? rep(coeff(h, i)) : ZZ(0); // Adds 0s if deg(h) < N
	}

	for (int i=0; i<2*N; i++) {
		for (int j=0; j<2*N; j++) {

			if (i < N) {
				if (j < N) {
					NTRU_lattice[i][j] = (i == j) ? ZZ(1) : ZZ(0); // Puts 1's on the diagonal in upper left
				} else {
					NTRU_lattice[i][j] = h_coeffs[(j-i) % N]; // Gets the rotations of h
				}
			} else {
				if (j < N) {
					NTRU_lattice[i][j] = 0;
				} else {
					NTRU_lattice[i][j] = (i == j) ? ZZ(q) : ZZ(0);
				}
			}

		}
	}
}

long NTRUBreak::reduce_lattice(int block_size) {

	// Run BKZ fill on NTRU lattice to try to recover key
	auto start = chrono::high_resolution_clock::now();
	BKZ_QP1(NTRU_lattice, 0.99, block_size);
	auto stop = chrono::high_resolution_clock::now();

	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start).count();

	return duration;
}

ZZX NTRUBreak::find_ternary(ZZ_pX& h) {
	set_modulus(q);

	ZZX row_poly;

	for (int row=0; row<NTRU_lattice.NumRows(); row++) {


		for (int i=0; i<N; i++) {
			ZZ val = NTRU_lattice[row][i];
			SetCoeff(row_poly, i, val);
		}

		if (is_ternary(center_lift(MulMod(reduce(row_poly), h, cyclo))) && is_ternary(row_poly)) {
			vec_ZZ row_vec = NTRU_lattice[row];
			ZZ norm;
			InnerProduct(norm, row_vec, row_vec);
			this->found_norm = norm;
			return row_poly;
		}

	}

	row_poly = ZZX();

	return row_poly;
}

bool NTRUBreak::is_rotation(ZZX& f, ZZX& g, int N) {
	vec_ZZ f_vec = f.rep;
	for (int i=f_vec.length(); i<N; i++) {
		f_vec.append(ZZ(0));
	}

	vec_ZZ g_vec = g.rep;
	for (int i=g_vec.length(); i<N; i++) {
		g_vec.append(ZZ(0));
	}

	bool not_found = false;
	for (int start=0; start<N; start++) {
		not_found = false;
		for (int i=0; i<N; i++) {
			if (f_vec[i] != g_vec[(start + i) % N]) {
				not_found = true;
				break;
			}
		}
		if (!not_found) {
			return true;
		}
	}

	return false;
}

void NTRUBreak::print_norms() {
	for (int i=0; i<NTRU_lattice.NumRows(); i++) {
		vec_ZZ row = NTRU_lattice[i];
		ZZ norm;
		InnerProduct(norm, row, row);
		cout << norm <<  "\n";
		if (norm <= this->found_norm*20) {
			cout << row << "\n";
		}
	}
}

ZZ NTRUBreak::get_smallest_norm() {
	ZZ smallest_norm;
	InnerProduct(smallest_norm, NTRU_lattice[0], NTRU_lattice[0]);
	for (int i=1; i<NTRU_lattice.NumRows(); i++) {
		ZZ norm;
		InnerProduct(norm, NTRU_lattice[i], NTRU_lattice[i]);
		if (norm < smallest_norm) smallest_norm = norm;
	}

	return smallest_norm;
}

