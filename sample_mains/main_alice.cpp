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


	// File for output
	// ofstream output_file;
	// output_file.open("Nq_tables_raw.csv");


	default_random_engine rand_seed = default_random_engine(time(0));

	int p = 3;
	// int Ns[] = {17, 53, 101, 257};
	int Ns[] = {101};
	// int qs[] = {32, 64, 128, 256};
	int qs[] = {128};
	int d;

	// output_file << "N,q,outcome\n";

	int trials = 1000;

	for (int i=0; i<trials; i++) {

		for (int N : Ns) {
			for (int q : qs) {

				bool finished_trial = false;

				while (!finished_trial) {

					d = N / 3;

					default_random_engine rand_gen = default_random_engine(rand_seed());

					try {	

						NTRUEncrypt ntru_encrypt(N, p, q, d, rand_gen);
						ntru_encrypt.gen_private_key();

						ZZX m = ntru_encrypt.gen_ternary(N, d+1, d);
						ZZ_pX e = ntru_encrypt.encrypt(m);

						ZZX m_recovered = ntru_encrypt.decrypt(e);

						bool is_rotation = NTRUBreak::is_rotation(m, m_recovered, N);

						string outcome;
						if (!is_rotation) {
							ZZ width = ntru_encrypt.get_width(m);
							if (width < q) {
								outcome = "Wrap_Failure";
							} else {
								outcome = "Gap Failure";
							}
						} else {
							outcome = "Success";
						}

						// cout << "N: " << N << " q: " << q << "Is rotation: " << NTRUBreak::is_rotation(m, m_recovered, N) << "\n";
						// output_file << N << "," << q << "," << outcome << "\n";

						finished_trial = true;

					} catch (ArithmeticErrorObject e) {
						// cout << e.what() << "\n";
					}

				}
			}
		}

	}

	// output_file.close();

}