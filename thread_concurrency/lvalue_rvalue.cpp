
#include <vector>
#include <iostream>


void
/*
 * &&, accept rvalue only
 */
process_copy(std::vector<int> && vec)
{
        vec.push_back(10);
        for(auto i : vec)
                std::cout << i << " ";
        std::cout << std::endl;
}


void
/*
 * &, accept reference
 */
process_copy(std::vector<int> & vec)
{
        vec.push_back(9);
        for(auto i : vec)
                std::cout << i << " ";
        std::cout << std::endl;
}



/*
 * T&&
 *       indicates an rvalue reference to type, T
 * or an reference to type, T
 */
template<class T>
void process_copy_2(T&& t)
{
        t++;
        std::cout << __func__ << " " << t << std::endl;
}


int
main(int argc, char* argv[])
{
        process_copy(std::vector<int> {1, 2, 3, 4});
        std::vector<int> v {5, 6, 7, 8, 9};
        process_copy(v);

        /* rvalue reference cannot be constructed an lvalue
         * int k {2};
         * int && rrk {k};
        */

        /* test rvalue */
        process_copy_2(1);

        /* test rvalue reference */
        int &&i {2};
        process_copy_2(i);
        i++;
        std::cout << __func__ << " " << i << std::endl;

        /* test lvalue reference */
        int j {3};
        process_copy_2(j);
        std::cout << __func__ << " " << j << std::endl;
        return 0;
}
