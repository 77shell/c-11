
#include <iostream>
#include "impl.h"
#include <deque>
#include <vector>

struct Gadget {
        std::deque<int> data;
};

struct Widget::impl {
        std::string name;
        std::vector<int> data;
        Gadget g1, g2, g3;
};

Widget::Widget()
        : pImpl {std::make_unique<impl>()}
          {
                  std::cout << __func__ << " : constructor\n";
          }

Widget::~Widget()
{
        std::cout << __func__ << std::endl;
}


Widget::Widget(Widget &&rhs)
{
        std::cout << __func__ << " : move constructor\n";
        if(rhs.pImpl)
                pImpl = std::make_unique<impl>(*rhs.pImpl);
}


Widget&
Widget::operator=(const Widget &rhs)
{
        std::cout << __func__ << " : assignment operator\n";
        if(pImpl) {
                if(rhs.pImpl)
                        *pImpl = *rhs.pImpl;
                else
                        std::cout << "rhs.pImpl is empty." << std::endl;
        }
        else
                pImpl = std::make_unique<impl>(*rhs.pImpl);
        
        
                
}
