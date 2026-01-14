#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <string>
#include <vector>

class account {
	std::mutex m;
	std::string name;
	int balance;
public:
	account() : name{"Human"}, balance{0} {};
	account(const std::string& name, int balance) : name{name}, balance{balance} {};

	std::mutex& getMutex() { return m; };
	const std::string& getName() const { return name; };
	const int getBalance() const { return balance; };

	void deposit(int amount) { balance += amount; };
	void withdraw(int amount) { balance -= amount; };
};

void deposit(std::promise<std::string> p, account& acc, int amount) {
	std::string s;
	{
		std::lock_guard<std::mutex> lock(acc.getMutex());
		acc.deposit(amount);
		s = acc.getName() + ": deposited " + std::to_string(amount) + " -> new balance " + std::to_string(acc.getBalance());
	}
	p.set_value(s);
}

void withdrawal(std::promise<std::string> p, account& acc, int amount) {
	std::string s;
	{
		std::lock_guard<std::mutex> lock(acc.getMutex());
		acc.withdraw(amount);
		s = acc.getName() + ": withdrew " + std::to_string(amount) + " -> new balance " + std::to_string(acc.getBalance());
	}
	p.set_value(s);
}

int main() {
	account acc1{"Jay", 1000};
	account acc2{"Joe", 3000};
	account acc3{"Tommy", 1500};

	std::vector<std::future<std::string>> futures;
	std::vector<std::thread> threads;

	std::promise<std::string> p;
	futures.push_back(p.get_future());
	threads.push_back(std::thread(deposit, std::move(p), std::ref(acc1), 200));

	std::promise<std::string> p2;
	futures.push_back(p2.get_future());
	threads.push_back(std::thread(deposit, std::move(p2), std::ref(acc2), 50));

	std::promise<std::string> p3;
	futures.push_back(p3.get_future());
	threads.push_back(std::thread(deposit, std::move(p3), std::ref(acc3), 10));

	std::promise<std::string> p4;
	futures.push_back(p4.get_future());
	threads.push_back(std::thread(deposit, std::move(p4), std::ref(acc1), 200));

	std::promise<std::string> p5;
	futures.push_back(p5.get_future());
	threads.push_back(std::thread(deposit, std::move(p5), std::ref(acc2), 200));

	std::promise<std::string> p6;
	futures.push_back(p6.get_future());
	threads.push_back(std::thread(deposit, std::move(p6), std::ref(acc3), 200));

	std::promise<std::string> p7;
	futures.push_back(p7.get_future());
	threads.push_back(std::thread(withdrawal, std::move(p7), std::ref(acc1), 20));

	std::promise<std::string> p8;
	futures.push_back(p8.get_future());
	threads.push_back(std::thread(withdrawal, std::move(p8), std::ref(acc2), 50));

	std::promise<std::string> p9;
	futures.push_back(p9.get_future());
	threads.push_back(std::thread(withdrawal, std::move(p9), std::ref(acc3), 20));

	std::promise<std::string> p10;
	futures.push_back(p10.get_future());
	threads.push_back(std::thread(withdrawal, std::move(p10), std::ref(acc1), 50));

	std::promise<std::string> p11;
	futures.push_back(p11.get_future());
	threads.push_back(std::thread(withdrawal, std::move(p11), std::ref(acc2), 100));

	std::promise<std::string> p12;
	futures.push_back(p12.get_future());
	threads.push_back(std::thread(withdrawal, std::move(p12), std::ref(acc3), 60));

	for (std::thread& t : threads) {
		t.join();
	}

	for (std::future<std::string>& f : futures) {
		std::cout << f.get() << std::endl;
	}

	return 0;
}