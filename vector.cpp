
#include <iostream>
#include <vector>
#include <unordered_map>
#include <atomic>

using namespace std;

struct A;
std::unordered_map<int, A*> A_map;

struct A {
        A(const char *name, int id)
                : s {name}, i {id}
                {
                        A_map.insert({id, this});
                }

        ~A()
                {
                        //A_map.erase(i);
                }

        std::string s;
        int i;
};

std::vector<A> vec_A;

int main(int argc, char *argv[])
{
        vec_A.push_back(A {"hello", 1});
        
        cout << "Map size: " << A_map.size() << endl;
        for(auto i : A_map)
        {
                cout << "Map entry: " << i.first << endl;
        }

	vector<atomic<uint16_t>> vec_B(4, static_cast<uint16_t>(0xffff));
	cout << "vector size() " << vec_B.size() << endl;
                
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
        fprintf(stdout, "txq size: %d\n", txq.size());
}
