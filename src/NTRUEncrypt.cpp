#include "NTRUEncrypt.h"


using namespace std;
using namespace NTL;

NTRUEncrypt::NTRUEncrypt() {}

NTRUEncrypt::NTRUEncrypt(int N, int p, int q, int d) {
	this->N = N;
	this->p = p;
	this->q = q;
	this->d = d;

	rand_gen = default_random_engine(time(0));
}

NTRUEncrypt::NTRUEncrypt(int N, int p, int q, int d, default_random_engine& rand) {
	this->N = N;
	this->p = p;
	this->q = q;
	this->d = d;

	rand_gen = rand;
}

// Reduces a polynomial from ZZX for ZZ_pX
ZZ_pX NTRUEncrypt::reduce(const ZZX& f) {
	ZZ_pX f_modp;
	stringstream buffer;
	buffer << f;
 	buffer >> f_modp;
 	return f_modp;
}

// Reduces a polynomial from ZZX for ZZ_pX
ZZ_pX NTRUEncrypt::reduce(const ZZ_pX& f) {
	// Center lift f
	ZZ one = ZZ(1);
	ZZX f_lift;
	CRT(f_lift, one, f);

	return reduce(f_lift);
}

// Center lifts a polynomial from ZZ_pX to ZZX
ZZX NTRUEncrypt::center_lift(const ZZ_pX& f) {
	ZZ prod = ZZ(1);
	ZZX f_lift;
	CRT(f_lift, prod, f);
	return f_lift;
}

ZZX NTRUEncrypt::center_lift(const ZZ_pX& f, int low, int high) {
	ZZX f_lift;
	f_lift.SetLength(f.rep.length());
	ZZ mod = ZZ_p::modulus();

	for (int i=0; i<f.rep.length(); i++) {
		ZZ val = rep(f[i]);
		if (val <= low) {
			f_lift[i] = val + mod;
		} else if (val > high) {
			f_lift[i] = val - mod;
		} else {
			f_lift[i] = val;
		}
	}

	return f_lift;
}

ZZX NTRUEncrypt::gen_ternary(int N, int d1, int d2) {
	ZZX f;
	f.SetLength(N);

	vector<int> f_coeffs;
	for (int i=0; i<N; i++) {
		if (i < d1) {
			f_coeffs.push_back(1);
		} else if (i < d1 + d2) {
			f_coeffs.push_back(-1);
		} else {
			f_coeffs.push_back(0);
		}
	}

	shuffle(f_coeffs.begin(), f_coeffs.end(), rand_gen);


	for (int i=0; i<N; i++) {
		SetCoeff(f, i, f_coeffs[i]);
	}

	return f;
}

void NTRUEncrypt::gen_private_key() {
	f = gen_ternary(N, d+1, d);
	g = gen_ternary(N, d, d);
}

void NTRUEncrypt::set_modulus(int p) {
	ZZ_p::init(ZZ(p));
	cyclo = ZZ_pX(INIT_MONO, N) - 1;
}


ZZ_pX NTRUEncrypt::inverse2k(const ZZ_pX& f) {

	// Find k such that q = 2^k
	int x = q;
	int k = 0;
	while (true) {
		x = x / 2;
		k++;
		if (x == 1) break;
		if (x % 2 != 0) {
			cout << "q is not a power of 2\n";
			exit(0);
		}
	}

	// Center lift f
	// ZZX a_lift = center_lift(f);

	// Map to a mod 2 polynomial
	GF2X f_mod2;
	stringstream buffer;
	buffer << f;
 	buffer >> f_mod2;

 	// Find the inverse mod 2
 	GF2X cyclo_mod2 = GF2X(INIT_MONO, N) - 1;
 	GF2X f_inv2 = InvMod(f_mod2, cyclo_mod2);

 	int mod = 2;
 	ZZ_p::init(ZZ(mod));

 	// Convert the inverse of f to a ZZ_pX (still mod 2)
 	ZZ_pX f_inv;
	stringstream buffer2;
	buffer2 << f_inv2;
 	buffer2 >> f_inv;

 	// Run Almost Inverses Algorithm
 	ZZ_pX cyclo;
 	while (mod < pow(2.0, (double)k)) {

 		mod *= mod;
 		ZZ_p::init(ZZ(mod));
 		cyclo = ZZ_pX(INIT_MONO, N) - 1;
 		f_inv = MulMod(f_inv, 2 - MulMod(f, f_inv, cyclo), cyclo);
 	}


 	ZZ_p::init(ZZ(q));
 	f_inv = reduce(f_inv);
 	return f_inv;
}


ZZ_pX NTRUEncrypt::encrypt(ZZX m) {

	set_modulus(q);

	ZZ_pX f_modq = reduce(f);
	ZZ_pX g_modq = reduce(g);

	ZZ_pX f_invq = inverse2k(f_modq);
	ZZ_pX h = MulMod(f_invq, g_modq, cyclo);

	this->r = reduce(gen_ternary(N, d, d));

	ZZ_pX e = (p * MulMod(r, h, cyclo)) + reduce(m);

	return e;
}


ZZX NTRUEncrypt::decrypt(ZZ_pX& e) {
	ZZ_pX f_modq = reduce(f);

	ZZ_pX a = MulMod(f_modq, e, cyclo);

	ZZX a_lift = center_lift(a);

	set_modulus(p);

	ZZ_pX a_modp = reduce(a_lift);
	ZZ_pX f_modp = reduce(f);

	ZZ_pX b = MulMod(InvMod(f_modp, cyclo), a_modp, cyclo);
	ZZX m_recovered = center_lift(b);

	set_modulus(q);

	return m_recovered;
}

ZZX NTRUEncrypt::decrypt(ZZ_pX& e, ZZX& f_prime) {

	ZZ_pX f_modq = reduce(f_prime);

	ZZ_pX a = MulMod(f_modq, e, cyclo);

	ZZX a_lift = center_lift(a);

	set_modulus(p);

	ZZ_pX a_modp = reduce(a_lift);
	ZZ_pX f_modp = reduce(f_prime);

	ZZ_pX b = MulMod(InvMod(f_modp, cyclo), a_modp, cyclo);
	ZZX m_recovered = center_lift(b);

	set_modulus(q);

	return m_recovered;
}

ZZX NTRUEncrypt::decrypt(ZZ_pX& e, int low, int high) {
	ZZ_pX f_modq = reduce(f);

	ZZ_pX a = MulMod(f_modq, e, cyclo);

	ZZX a_lift = center_lift(a, low, high);

	set_modulus(p);

	ZZ_pX a_modp = reduce(a_lift);
	ZZ_pX f_modp = reduce(f);

	ZZ_pX b = MulMod(InvMod(f_modp, cyclo), a_modp, cyclo);
	ZZX m_recovered = center_lift(b);

	set_modulus(q);

	return m_recovered;
}

ZZ_pX NTRUEncrypt::get_public_key() {
	return MulMod(inverse2k(reduce(f)), reduce(g), cyclo);
}


ZZ NTRUEncrypt::get_width(ZZX& m) {
	ZZX cyclo_tmp = ZZX(INIT_MONO, N) - 1;
	ZZX a = MulMod(f, m, cyclo_tmp) + (p * MulMod(center_lift(r), g, cyclo_tmp));

	ZZ max = a[0];
	ZZ min = a[0];
	for (int i=1; i<a.rep.length(); i++) {
		if (a[i] > max) max = a[i];
		if (a[i] < min) min = a[i];
	}

	return max - min;
}
