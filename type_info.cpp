#include <iostream>
#include <unordered_map>
#include <typeindex>
#include <map>
#include <vector>
#include "db/datum.h"

struct A {
        A(int j)
                : i {j} 
                {
                        map.insert({i, this});
                }
        int i;
        static std::unordered_map<int,A*> map;
};

std::unordered_map<int,A*> A::map;

struct AA {
        AA()
                : a {1}, b{2}
                {}
        A a;
        A b;
};


AA aa;

struct B {
   virtual std::type_index type_id() = 0;
   virtual std::string name() = 0;
};

struct B1 : public B {
   virtual std::type_index type_id()
      {
	 return std::type_index(typeid(B1));
      }
   virtual std::string name()
      {
	 return std::string {"B1"};
      }
};

struct B2 : public B {
   virtual std::type_index type_id()
      {
	 return std::type_index(typeid(B2));
      }
   virtual std::string name()
      {
	 return std::string {"B2"};
      }
};

B1 b1;
B2 b2;

std::map<std::type_index, B*> typeid_map {
   { std::type_index(typeid(B1)), &b1 },
   { std::type_index(typeid(B2)), &b2 }
};

int
main(int argc, char *argv[])
{
	using namespace std;
        std::cout << typeid(A::map[1]).name() << std::endl;
	B *pB {&b1};
	std::cout << typeid_map[pB->type_id()]->name() << std::endl;

	std::vector<std::type_index> vec_t { typeid(int), typeid(short), typeid(Database::DATUM<u32>) };
	for (auto &t : vec_t) {
		cout << "Name: " << t.name() << "\thash code: " << hex << t.hash_code() << endl;
	}
        return 0;
}
