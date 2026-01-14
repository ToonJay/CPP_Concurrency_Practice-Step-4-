#include <iostream>
#include <vector>
#include <chrono>

int main() {
	auto start = std::chrono::steady_clock::now();
	std::vector<long long> vec;
	vec.reserve(1000000000);
	std::vector<long long> partialSums;
	long long partialSum{0};
	long long startingIndex;

	for (long long i = 0; i < 1000000000; i++) {
		vec.push_back(i + 1);
	}

	for (size_t i = 0; i < 10; i++) {
		startingIndex = i * 100000000;
		for (size_t j = startingIndex; j < startingIndex + 100000000; j++) {
			partialSum += vec.at(j);
		}
		partialSums.push_back(partialSum);
		partialSum = 0;
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