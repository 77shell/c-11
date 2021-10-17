
#include <initializer_list>
#include <iostream>
#include <thread>
#include <map>
#include <vector>

using namespace std;

void f(initializer_list<unsigned int> lst)
{
        //for(int i=0; i<lst.size(); i++)
        // cout << lst[i] << endl;

        const unsigned int *p = lst.begin();
        for(unsigned int i=0; i<lst.size(); i++)
                cout << p[i] << endl;
}


void f2(initializer_list<unsigned int> lst)
{
        for(auto x : lst)
                cout << x << " ";
}


void f3()
{
        for(auto x : { 1, 2, 3, 4 })
                cout << x << " ";
}


struct S_t {
	S_t(int idx) {
		pidx = new int {idx};
		std::cout << __func__ << " ctor " << pidx << "@" << *pidx << endl;
	}

	S_t(S_t &&s) {
		pidx = s.pidx;
		s.pidx = 0;
		std::cout << __func__ << " move ctrl " << pidx << "@" << *pidx << endl;
	}

	~S_t() {
		std::cout << __func__ << " dtor " << pidx;
		if(pidx) {
			cout << " perform delete";
			delete pidx;
		}
		else {
			cout << " skip delete";
		}
		cout << endl;
	}

	int *pidx;
};

struct reg_table {
	reg_table() {
		for(int i=0; i<1; i++)
			vec.push_back(S_t {i});
		//vec.push_back(S_t {2});
	}

	void print() {
		cout << "vector size: " << vec.size() << endl;
		cout << "vector cap: " << vec.capacity() << endl;
		cout << __func__ << endl;
		for(auto &v : vec)
			cout << v.pidx << " ";
		cout << endl;
	}
	std::vector<S_t> vec;
};

int
main(int argc, char *argv[])
{
        initializer_list<unsigned int> lst = { 0, 1, 2, 3 };
        f(lst);
        f2(lst);
        f3();

	for(int i=0; i<3; i++)
		for(auto i : lst)
			cout << i;
	
        std::cout << "Hello world" << std::endl;

	//std::map<int, S_t&> s_map;
	//std::vector<S_t> s_vec;
	{
		reg_table reg;
		reg.print();

		// initializer_list<S_t> s_list {S_t {1}, S_t {2}};
		// for(S_t& it = s_list.begin(); it != s_list.end(); it++)
		// 	std::cout << it.pidx << std::endl;

		// for(S_t* it = s_list.begin(); it != s_list.end(); it++)
		// 	s_vec.push_back(std::move(*it));

		// int i {0};
		// for(auto *s : s_list)
		// 	s_map.emplace(std::pair<int,S_t*> {i++, s});
		// for(auto it=s_map.begin(); it != s_map.end(); it++)
		// 	cout << it->first << "\t" << *it->second.pidx << endl;
		this_thread::sleep_for(chrono::seconds {2});
	}
	this_thread::sleep_for(chrono::seconds {2});
	
        exit(EXIT_SUCCESS);
}
