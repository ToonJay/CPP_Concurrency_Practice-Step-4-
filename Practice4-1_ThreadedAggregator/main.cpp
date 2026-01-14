/*
#include <iostream>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <chrono>

void partialSum(std::vector<long long>& partialSums, std::vector<long long>& vec, size_t startingIndex, std::mutex& m) {
	long long partialSum{0};

	for (size_t i = startingIndex; i < startingIndex + 100000000; i++) {
		partialSum += vec.at(i);
	}

	std::lock_guard<std::mutex> lg(m);
	partialSums.push_back(partialSum);
}

int main() {
	auto start = std::chrono::steady_clock::now();
	std::mutex m;
	std::vector<long long> vec;
	vec.reserve(1000000000);
	std::vector<long long> partialSums;
	std::vector<std::thread> threads;

	for (long long i = 0; i < 1000000000; i++) {
		vec.push_back(i + 1);
	}

	for (size_t i = 0; i < 10; i++) {
		threads.emplace_back(std::thread(partialSum, std::ref(partialSums), std::ref(vec), i * 100000000, std::ref(m)));
	}

	for (std::thread& t : threads) {
		t.join();
	}

	long long totalSum{0};

	for (long long i : partialSums) {
		totalSum += i;
	}
	auto stop = std::chrono::steady_clock::now();
	std::cout << "Total: " << totalSum << std::endl;
	std::cout << "Elapsed Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start) << std::endl;

	return 0;
}
*/