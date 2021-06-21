#include <map>
#include <functional>
#include <iostream>
 
struct Foo {
        Foo(int num) : num_(num) {}
        void print_add(int i) const { std::cout << num_+i << '\n'; }
        int num_;
};
 
void print_num(int i)
{
        std::cout << i << '\n';
}
 
struct PrintNum {
        void operator()(int i) const
                {
                        std::cout << i << '\n';
                }
};

struct S_t {
	enum T_e {
		eT1,
		eT2
	};
	void f1(int x, int y) {
		std::cout << __func__ << "\t" << x << "\t" << y << std::endl;
	}
	void f2(int x, int y) {
		std::cout << __func__ << "\t" << x << "\t" << y << std::endl;
	}
};
 
int main()
{
	S_t s;
	std::function<void(int, int)>
		pf_f1 {std::bind(&S_t::f1, &s, std::placeholders::_1, std::placeholders::_2)},
		pf_f2 {std::bind(&S_t::f2, &s, std::placeholders::_1, std::placeholders::_2)};
	std::map<S_t::T_e, std::function<void(int, int)>> f_map {
		{S_t::eT1, pf_f1},
		{S_t::eT2, pf_f2}
	};
	f_map[S_t::eT1](1, 2);
	f_map[S_t::eT2](2, 3);

        // store a free function
        std::function<void(int)> f_display = print_num;
        f_display(-9);
 
        // store a lambda
        std::function<void()> f_display_42 = []() { print_num(42); };
        f_display_42();
 
        // store the result of a call to std::bind
        std::function<void()> f_display_31337 = std::bind(print_num, 31337);
        f_display_31337();
 
        // store a call to a member function
        std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
        const Foo foo(314159);
        f_add_display(foo, 1);
        f_add_display(314159, 1);
 
        // store a call to a data member accessor
        std::function<int(Foo const&)> f_num = &Foo::num_;
        std::cout << "num_: " << f_num(foo) << '\n';
 
        // store a call to a member function and object
        using std::placeholders::_1;
        std::function<void(int)> f_add_display2 = std::bind( &Foo::print_add, foo, _1 );
        f_add_display2(2);
 
        // store a call to a member function and object ptr
        std::function<void(int)> f_add_display3 = std::bind( &Foo::print_add, &foo, _1 );
        f_add_display3(3);
 
        // store a call to a function object
        std::function<void(int)> f_display_obj = PrintNum();
        f_display_obj(18);        
}

