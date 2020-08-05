#include "../src/NTRUEncrypt.h"
#include "../src/NTRUBreak.h"
#include <fstream>


ZZ find_max(ZZX& f) {
	ZZ max = f[0];
	for (int i=1; i<f.rep.length(); i++) {
		if (f[i] > max) max = f[i];
	}
	return max;
}

ZZ find_min(ZZX& f) {
	ZZ min = f[0];
	for (int i=1; i<f.rep.length(); i++) {
		if (f[i] < min) min = f[i];
	}
	return min;
}



int main() {


	default_random_engine rand_seed = default_random_engine(time(0));

	int p = 3;
	int Ns[] = {41, 47, 53, 61, 67, 73, 79, 83};
	int qs[] = {128};
	int divs[] = {3};
	int d;
	int block_sizes[] = {2, 4, 6, 10, 15};


	int trials = 40;

	for (int i=0; i<trials; i++) {
		cout << "i: " << i << "\n";
		for (int j=0; j<sizeof(Ns)/sizeof(Ns[0]); j++) {
			int N = Ns[j];
			for (int div : divs) {
				for (int q : qs) {
					d = N / div;
					default_random_engine rand_gen = default_random_engine(rand_seed());

					bool finished = false;

					while (!finished) {

						try {	

							NTRUEncrypt ntru_encrypt(N, p, q, d, rand_gen);
							ntru_encrypt.gen_private_key();

							ZZX m = ntru_encrypt.gen_ternary(N, d+1, d);
							ZZ_pX e = ntru_encrypt.encrypt(m);

							ZZ smallest_norm = ZZ(0);
							ZZ prev_norm;

							int block_size = block_sizes[j];

							while (true) {

								// cout << "Block Size: " << block_size << "\n";

								ZZ_pX pub_key = ntru_encrypt.get_public_key();
								NTRUBreak ntru_break(N, p, q, pub_key);

								long time = ntru_break.reduce_lattice(block_size);

								ZZX f_recovered = ntru_break.find_ternary(pub_key);
								if (deg(f_recovered) >= 0) {
									cout << N << "," << q << "," << d << "," << block_size << "," << time << "\n";
									break;

								} else {
									block_size++;
								}

							}

							finished = true;

						} catch (ArithmeticErrorObject e) {
							// cout << e.what() << "\n";
						}

					}

				}
			}
		}
	}

}