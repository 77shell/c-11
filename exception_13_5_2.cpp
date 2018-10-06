
#include <iostream>


using namespace std;

class E {
public:
   E() {}
   virtual void do_something(void) { std::cout << "E: Do something" << std:: endl; }
};


class F : public E {
public:
   F() {}
   virtual void do_something(void) { std::cout << "F: Do something" << std::endl; }
};
   

void f()
{
   std::cout << "Hello" << std::endl;
   throw F{};
}


int main(int argc, char *argv[])
   try {
      f();
      exit(EXIT_SUCCESS);
   }
   catch(E obj) {
      obj.do_something();
   }
   catch(E) {
      cout << "Catch you" << endl;
      exit(EXIT_SUCCESS);
   }
   catch(F obj) {
      obj.do_something();
   }
   catch(...) {
      cout << "Catch everything" << endl;
   }

