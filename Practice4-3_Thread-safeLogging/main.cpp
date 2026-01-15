#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <chrono>

void logger_producer(std::string name, std::queue<std::string>& message_q, std::mutex& m, std::condition_variable& cv, bool& shutdown) {
	int i{0};
	while (true) {
		{
			std::lock_guard<std::mutex> lg(m);
			if (shutdown) {
				break;
			}
			message_q.emplace(name + ": Writing message " + std::to_string(++i) + ".");
		}
		cv.notify_one();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void logger_consumer(std::queue<std::string>& message_q, std::mutex& m, std::condition_variable& cv, bool& shutdown) {
	std::ofstream out_file{"./shared_log.txt"};
	std::string message;
	if (out_file) {
		while (true) {
			{
				std::unique_lock<std::mutex> ul(m);
				cv.wait(ul, [&]() { return !message_q.empty() || shutdown; });
				if (shutdown && message_q.empty()) {
					break;
				}
				message = message_q.front();
				message_q.pop();
			}
			out_file << message << "\n";
		}
	} else {
		std::cout << "Error opening output file!" << std::endl;
	}

	out_file.close();
}

int main() {
	std::queue<std::string> message_q;
	std::mutex m;
	std::condition_variable cv;
	std::vector<std::thread> threads;
	bool shutdown{false};
	std::string command{""};

	for (size_t i = 0; i < 10; i++) {
		threads.emplace_back(std::thread(logger_producer, "Producer " + std::to_string(i + 1), std::ref(message_q), std::ref(m), std::ref(cv), std::ref(shutdown)));
	}

	threads.emplace_back(std::thread(logger_consumer, std::ref(message_q), std::ref(m), std::ref(cv), std::ref(shutdown)));

	while (command != "end") {
		std::cin >> command;
	}

	{
		std::lock_guard<std::mutex> lg(m);
		shutdown = true;
	}
	cv.notify_all();

	for (std::thread& t : threads) {
		t.join();
	}

	return 0;
}