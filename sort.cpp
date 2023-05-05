#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int
main()
{
	vector<uint64_t> v {9, 6, 7, 8};
	std::sort(v.begin(), v.end());
	for (auto i : v)
		cout << i << ' ' << std::flush;
}
