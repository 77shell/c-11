
#include <atomic>
#include <iostream>
#include <unordered_map>
#include <map>
#include <stdexcept>

using namespace std;

enum Device_e {
	eA,
	eB,
	eC,
	eD
};

struct DevInfo_t {
	DevInfo_t(size_t i)
		{
			counter = i;
		}
	std::atomic<size_t> counter;
};

DevInfo_t A {1};
DevInfo_t B {2};
DevInfo_t C {3};

std::map<Device_e, DevInfo_t*> devmap {
	{ eA, &A },
	{ eB, &B },
	{ eC, &C }
};


std::map<Device_e, DevInfo_t*> devmap2 {
	{ eA, new DevInfo_t {2} },
	{ eB, new DevInfo_t {3} },
	{ eC, new DevInfo_t {4} }
};

DevInfo_t*
get_dev_info(Device_e d)
{
#if 0
	try {
		return devmap.at(d);
	}
	catch(out_of_range &r) {
		cout << r.what() << endl;
		return nullptr;
	}
#else
	return devmap.at(d);
#endif
}

int main(int argc, char *argv)
{
	for(auto &i : devmap)
		cout << "First: " << i.first << "\tCounter: " << i.second->counter.load() << endl;
	DevInfo_t *info {devmap[eA]};
	cout << info->counter.load() << endl;;
	cout << "eB\t" << devmap.at(eB)->counter.load() << endl;
	cout << "eB\t" << get_dev_info(eB)->counter.load() << endl;

	try {
		cout << "eD\t" << get_dev_info(eD)->counter.load() << endl;
	}
	catch(out_of_range &r) {
		cout << r.what() << endl;
	}

	for(auto &i : devmap2)
		cout << "First: " << i.first << "\tCounter: " << i.second->counter.load() << endl;
	return 0;
}
