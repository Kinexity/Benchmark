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
	std::uniform_real_distribution<> dis_f64(std::nextafter(0.0, 1.0), 1.0);
	std::uniform_real_distribution<float> dis_f32(std::nextafter(0.0, 1.0), 1.0);
	size_t N = 1e7;
	std::vector<float>
		v1_f32, v2_f32, v3_f32, res_f32;
	std::vector<double>
		v1_f64, v2_f64, v3_f64, res_f64;
	bool vectorize = false;
	res_f64.resize(N);
	res_f32.resize(N);
	for (auto i = 0; i < N; i++) {
		v1_f64.push_back(dis_f64(gen));
		v2_f64.push_back(dis_f64(gen));
		v3_f64.push_back(dis_f64(gen));
		v1_f32.push_back(dis_f32(gen));
		v2_f32.push_back(dis_f32(gen));
		v3_f32.push_back(dis_f32(gen));
	}
	std::cout << "Benchmark results:\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 8) {
		_mm256_store_ps(&res_f32[i], _mm256_sin_ps(_mm256_load_ps(&v1_f32[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f32[i] = std::sin(v1_f32[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result sin (f32): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res_f64[i], _mm256_sin_pd(_mm256_load_pd(&v1_f64[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f64[i] = std::sin(v1_f64[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result sin (f64): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 8) {
		_mm256_store_ps(&res_f32[i], _mm256_sqrt_ps(_mm256_load_ps(&v1_f32[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f32[i] = std::sqrt(v1_f32[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result sqrt (f32): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res_f64[i], _mm256_sqrt_pd(_mm256_load_pd(&v1_f64[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f64[i] = std::sqrt(v1_f64[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result sqrt (f64): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 8) {
		_mm256_store_ps(&res_f32[i], _mm256_div_ps(_mm256_load_ps(&v1_f32[i]), _mm256_load_ps(&v2_f32[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f32[i] = v1_f32[i] / v2_f32[i];
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result div (f32): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res_f64[i], _mm256_div_pd(_mm256_load_pd(&v1_f64[i]), _mm256_load_pd(&v2_f64[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f64[i] = v1_f64[i] / v2_f64[i];
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result div (f64): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 8) {
		_mm256_store_ps(&res_f32[i], _mm256_log_ps(_mm256_load_ps(&v1_f32[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f32[i] = std::log(v1_f32[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result log (f32): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res_f64[i], _mm256_log_pd(_mm256_load_pd(&v1_f64[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f64[i] = std::log(v1_f64[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result log (f64): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 8) {
		_mm256_store_ps(&res_f32[i], _mm256_fmadd_ps(_mm256_load_ps(&v1_f32[i]), _mm256_load_ps(&v2_f32[i]), _mm256_load_ps(&v3_f32[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f32[i] = std::fma(v1_f32[i], v2_f32[i], v3_f32[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result fma (f32): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
#if defined(__amd64__) || defined(_M_X64)
	for (auto i = 0; i < N; i += 4) {
		_mm256_store_pd(&res_f64[i], _mm256_fmadd_pd(_mm256_load_pd(&v1_f64[i]), _mm256_load_pd(&v2_f64[i]), _mm256_load_pd(&v3_f64[i])));
	}
#else
	for (auto i = 0; i < N; i++) {
		res_f64[i] = std::fma(v1_f64[i], v2_f64[i], v3_f64[i]);
	}
#endif // __amd64__
	tc.stop();
	std::cout << "Result fma (f64): " << N / tc.measured_timespan().count() << "elems/s\n";
}