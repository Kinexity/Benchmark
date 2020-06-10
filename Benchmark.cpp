#include <iostream>
#include <chrono>
#include <iostream>
#include <execution>
#include <algorithm>
#include <numeric>
#include <random>
#include <vector>
#include <thread>

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
	size_t N;
	std::cout << "N = ";
	std::cin >> N;
	std::vector<double>
		v1, v2, v3, res;
	std::vector<size_t>
		indexes;
	res.resize(N);
	indexes.resize(N);
	std::iota(indexes.begin(), indexes.end(), size_t());
	for (auto i : indexes) {
		v1.push_back(dis(gen));
		v2.push_back(dis(gen));
		v3.push_back(dis(gen));
	}
	std::cout << "Threads: " << std::thread::hardware_concurrency() << '\n';
	tc.start();
	std::for_each(std::execution::par, indexes.begin(), indexes.end(), [&](size_t i) { res[i] = std::sin(v1[i] * v2[i] + v3[i]); });
	tc.stop();
	std::cout << "Result (multicore): " << N / tc.measured_timespan().count() << "elems/s\n";
	tc.start();
	std::for_each(std::execution::seq, indexes.begin(), indexes.end(), [&](size_t i) { res[i] = std::sin(v1[i] * v2[i] + v3[i]); });
	tc.stop();
	std::cout << "Result (singlecore): " << N / tc.measured_timespan().count() << "elems/s\n";
}