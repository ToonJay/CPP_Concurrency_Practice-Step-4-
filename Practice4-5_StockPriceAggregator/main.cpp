#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>

using namespace std::chrono_literals;

void fetch_stock_prices(std::promise<bool> p, std::mutex& m, std::vector<double>& prices) {
	auto start = std::chrono::steady_clock::now();

	std::mt19937 rng{std::random_device{}()};
	std::uniform_real_distribution<double> price_dist(100.0, 300.0);
	std::uniform_int_distribution<int> delay_dist(0, 200);

	for (size_t i = 0; i < 3; i++) {
		if (std::chrono::steady_clock::now() - start >= 200ms) {
			p.set_value(false);
			return;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_dist(rng)));
		{
			std::lock_guard<std::mutex> lg(m);
			prices.push_back(price_dist(rng));
		}
	}

	p.set_value(true);
}

int main() {
	std::vector<std::thread> threads;
	std::mutex m;
	std::vector<double> prices;
	std::vector<std::future<bool>> futures;
	

	for (size_t i = 0; i < 10; i++) {
		std::promise<bool> p;
		futures.emplace_back(p.get_future());
		threads.emplace_back(std::thread(fetch_stock_prices, std::move(p), std::ref(m), std::ref(prices)));
	}

	for (size_t i = 0; i < 10; i++) {
		if (!futures[i].get()) {
			std::cout << "Thread " << i << " timed out!" << std::endl;
		}
	}

	for (auto& t : threads) {
		t.join();
	}

	int num_prices{(int)prices.size()};
	double total{0.0};
	for (const auto& p : prices) {
		total += p;
		std::cout << p << std::endl;
	}

	std::cout << "------------------------------" << std::endl;
	std::cout << "Number of prices: " << num_prices << std::endl;
	std::cout << "Stock Price Average: " << total / num_prices << std::endl;

	return 0;
}