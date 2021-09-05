#include <map>
#include <iostream>
using namespace std;
enum Event_e {
   eA,
   eB,
   eC,
   eD
};

int
main(int argc, char *argv[])
{
   std::map<Event_e, int> map {
      {eA, 10},
      {eB, 20},
      {eC, 30}
   };
   cout << map[eA] << endl;
//   cout << map[eD] << endl;
   auto r {map.find(eD)};
   if(r == map.end())
      cout << "Cannot find eD's value\n";

   auto b {map.find(eB)};
   if(b == map.end())
      cout << "Cannot find eB's value\n";
   else
      cout << "Key(eB)'s value " << b->second << endl;

   for(auto &i : map)
      cout << i.second << endl;
      

   cout << "Map size: " << map.size() << endl;

   cout << map[eA] << endl;
}
