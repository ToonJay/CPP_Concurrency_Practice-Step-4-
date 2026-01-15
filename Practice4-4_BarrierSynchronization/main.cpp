#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <functional>

class MyBarrier {
private:
	std::mutex m;
	std::condition_variable cv;
	int count = 0;
	int total;
	int generation = 0;
public:
	MyBarrier(int total) : total{total} {};

	void arrive_and_wait() {
		std::unique_lock<std::mutex> ul(m);
		count += 1;
		int my_generation = generation;
		if (count == total) {
			count = 0;
			generation += 1;
			cv.notify_all();
		} else {
			cv.wait(ul, [&]() { return my_generation != generation; });
		}
	}

	void arrive_and_drop() {
		{
			std::lock_guard<std::mutex> lg(m);
			total -= 1;
			cv.notify_all();
		}
	}
};

void test1(std::string& name, MyBarrier& b) {
	name = "Bob";
	b.arrive_and_wait();
	b.arrive_and_wait();
	name = "Jess";
	b.arrive_and_wait();
}

void test2(int& age, MyBarrier& b) {
	age = 25;
	b.arrive_and_wait();
	b.arrive_and_wait();
	age = 14;
	b.arrive_and_wait();
}

void test3(int& id, MyBarrier& b) {
	id = 6840301;
	b.arrive_and_wait();
	b.arrive_and_wait();
	id = 9706482;
	b.arrive_and_wait();
}

void test4(std::string& address, MyBarrier& b) {
	address = "123 Rainbow Road";
	b.arrive_and_wait();
	b.arrive_and_wait();
	address = "456 Dino Island";
	b.arrive_and_wait();
}

void test5(int& income, MyBarrier& b) {
	income = 50000;
	b.arrive_and_drop();
}

void test6(std::string& name, int& age, int& id, std::string& address, int& income, MyBarrier& b) {
	b.arrive_and_wait();
	if (name == "Bob" && age == 25 && id == 6840301 && address == "123 Rainbow Road" && income == 50000) {
		std::cout << "Verification successful" << std::endl;
		std::cout << "This is " << name << ", age " << age << ", id " << id << " who lives at " << address << " and makes " << income << " a year." << std::endl;
	}
	b.arrive_and_wait();
	b.arrive_and_wait();
	if (name == "Jess" && age == 14 && id == 9706482 && address == "456 Dino Island") {
		std::cout << "Verification successful" << std::endl;
		std::cout << "This is " << name << ", age " << age << ", id " << id << " who lives at " << address << "." << std::endl;
	}
}

int main() {
	MyBarrier b(6);
	std::vector<std::thread> threads;
	std::string name;
	int age;
	int id;
	std::string address;
	int income;

	threads.emplace_back(test1, std::ref(name), std::ref(b));
	threads.emplace_back(test2, std::ref(age), std::ref(b));
	threads.emplace_back(test3, std::ref(id), std::ref(b));
	threads.emplace_back(test4, std::ref(address), std::ref(b));
	threads.emplace_back(test5, std::ref(income), std::ref(b));
	threads.emplace_back(test6, std::ref(name), std::ref(age), std::ref(id), std::ref(address), std::ref(income), std::ref(b));

	for (std::thread& t : threads) {
		t.join();
	}

	return 0;
}