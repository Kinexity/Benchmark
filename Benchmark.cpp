#include <iostream>
#include <chrono>
#include <iostream>
//#include <execution>
#include <algorithm>
#include <numeric>
#include <random>
#include <vector>
#include <thread>
#if defined(__amd64__) || defined(_M_X64)
#include <intrin.h>
#else
#include <arm_neon.h>
#endif

namespace PCL {
	class C_Time_Counter {
	private:
		std::chrono::high_resolution_clock::time_point
			beg,
			end;
		bool
			counting_started = false;
	public:
		C_Time_Counter() = default;
		~C_Time_Counter() = default;
		inline void
			start(),
			stop();
		inline std::chrono::duration<double, std::ratio<1, 1>>
			measured_timespan() noexcept;
	};

	inline void C_Time_Counter::start() {
		if (counting_started) {
			throw std::runtime_error("Counting already started!");
		}
		else {
			beg = std::chrono::high_resolution_clock::now();
			counting_started = true;
		}
	}

	inline void C_Time_Counter::stop() {
		if (!counting_started) {
			throw std::runtime_error("Counting not started!");
		}
		else {
			end = std::chrono::high_resolution_clock::now();
			counting_started = false;
		}
	}

	inline std::chrono::duration<double, std::ratio<1, 1>> C_Time_Counter::measured_timespan() noexcept {
		return std::chrono::duration<double, std::ratio<1, 1>>(end - beg);
	}
}

inline PCL::C_Time_Counter tc;

int main() {
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(std::nextafter(0.0, 1.0), 1.0);
	size_t N = 1e7;
	std::vector<double>
		v1, v2, v3, res;
	bool vectorize = false;
	res.resize(N);
	for (auto i = 0; i < N; i++) {
		v1.push_back(dis(gen));
		v2.push_back(dis(gen));
		v3.push_back(dis(gen));
	}
	std::cout << "Benchmark results:\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res[i], _mm256_sin_pd(_mm256_load_pd(&v1[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res[i] = std::sin(v1[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result sin: " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res[i], _mm256_sqrt_pd(_mm256_load_pd(&v1[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res[i] = std::sqrt(v1[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result sqrt: " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res[i], _mm256_div_pd(_mm256_load_pd(&v1[i]), _mm256_load_pd(&v2[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res[i] = v1[i] / v2[i];
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result div: " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res[i], _mm256_log_pd(_mm256_load_pd(&v1[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res[i] = std::log(v1[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result log: " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start(); 
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res[i], _mm256_fmadd_pd(_mm256_load_pd(&v1[i]), _mm256_load_pd(&v2[i]), _mm256_load_pd(&v3[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res[i] = std::fma(v1[i], v2[i], v3[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result fma: " << N / tc.measured_timespan().count() << "elems/s\n";
}