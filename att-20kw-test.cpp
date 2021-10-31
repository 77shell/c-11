#include <iostream>
#include <atomic>
#include <cstdint>
#include <chrono>
#include <thread>
#include <future>
using namespace std::chrono_literals;

using modposi_t = uint8_t;
using u8 = uint8_t;
constexpr u8 msMax_SlotNbr {254};

void
position2xy(modposi_t p, u8 &shelf, u8 &slot)
{
        if(p == 0)
		return;

        constexpr u8 denominator {msMax_SlotNbr};
	shelf = 1;
	if(p > msMax_SlotNbr)
		shelf += p / denominator;

	int_fast8_t slt {p % denominator};
        slot = slt ? slt : msMax_SlotNbr;
	std::cout << "Shelf: " << std::dec << static_cast<int>(shelf) << "\t" << "Slot: " << std::dec << static_cast<int>(slot) << std::endl;
}

struct position_t {
	position_t()
		: stalePosi {true}, shelf {UINT8_MAX}, slot {UINT8_MAX}
		{
			rx_posidata.test_and_set();
		}

	void signal_posidata_arrive() {
		rx_posidata.clear(std::memory_order_acq_rel);
	}
	bool posidata_arrive() {
		return rx_posidata.test_and_set(std::memory_order_acq_rel) == false;
	}

	std::atomic<bool> stalePosi;
	std::atomic_flag rx_posidata;
	u8 shelf;
	u8 slot;
} mPosition;

int
main(int argc, char *argv[])
{
	u8 shelf, slot;
	position2xy(254, shelf, slot);
	position2xy(255, shelf, slot);
	position2xy(256, shelf, slot);

	auto h {std::async(std::launch::async, [&] {
						       int i;
						       std::cerr << "Signal thread starts ...\n";
						       std::cin >> i;
						       std::cerr << "Signal posidata arriving\n";
						       mPosition.signal_posidata_arrive();
						       std::this_thread::sleep_for(2s);
					       }
			)};
	for(;!mPosition.posidata_arrive();std::this_thread::sleep_for(1s)) {
		std::cerr << ".";
	}
	std::cerr << "Got posidata arrive signal\n";
}
