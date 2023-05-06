
#include <iostream>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <memory>
#include <initializer_list>

using namespace std;

struct A;
std::unordered_map<int, A*> A_map;

struct A {
        A(const char *name, int id)
                : s {name}, i {id}
                {
                        A_map.insert({id, this});
                }

	~A() {
		std::cout << __func__ << std::endl;
	}

        std::string s;
        int i;
};

std::vector<A> vec_A;

constexpr size_t ptr_size {5};
std::vector<std::unique_ptr<A>> vec_ptr(ptr_size);
static void test_2();

int main(int argc, char *argv[])
{
	test_2();

        vec_A.push_back(A {"hello", 1});
        
        cout << "Map size: " << A_map.size() << endl;
        for(auto i : A_map)
        {
                cout << "Map entry: " << i.first << endl;
        }


	vector<atomic<uint16_t>> vec_B(4); //, static_cast<uint16_t>(0xffff));
	cout << "vector size() " << vec_B.size() << endl;
	// Vector element: unique_ptr
	vec_ptr.at(0) = std::make_unique<A>("first", 0);
	cout << vec_ptr.at(0)->s << vec_ptr.at(0)->i << std::endl;
	try {
		vec_ptr.at(5) = std::make_unique<A>("ohno....", 3);
	}
	catch (const std::out_of_range &e) {
		std::cout << e.what() << std::endl;
	}

	if (!vec_ptr.at(1).get()) {
		std::cout << "Element-1 has not init yet\n";
		vec_ptr.at(1) = std::make_unique<A>("second", 1);
		A *a = vec_ptr.at(1).get();
	}

	std::vector<std::unique_ptr<A>>::iterator it {vec_ptr.begin()};
	cout << "Element-0 : " << (**it).s << endl;
	++it;
	cout << "Element-1 : " << (**it).s << endl;

	for (auto i = vec_ptr.begin(); i != vec_ptr.end(); i++)
		if (it->get()) {
			cout << "Element : " << (**it).s << endl;
		}
		else {
			cout << "Element has not initialzed yet\n";
		}
}

static void test_1()
{
	vector<int> v(5, 1);
	for(auto i : v)
		std::cout << "Element: " << i << std::endl;

        vector<char> txq;
        unsigned char d {0x31};
        for(ssize_t i=0; i<26; i++, d++)
                txq.push_back(d);
        fprintf(stdout, "%s\n", txq.data());
        fprintf(stdout, "txq size: %zd\n", txq.size());
}

static void test_2()
{
	cout << __func__ << endl;
	vector<int> vec {1, 2, 3, 4, 5};
	std::vector<int>::iterator iter = vec.begin();
	vec.resize(vec.capacity() + 1);
	for (auto i : vec)
		cout << i;
	cout << endl << __func__ << " end\n";
}
