#ifndef _IMPL_H
#define _IMPL_H

#include <memory>

class Widget {
public:
        Widget();
        Widget(Widget &&);
        Widget& operator=(const Widget &rhs);
        ~Widget();
        

private:
        struct impl;
        std::unique_ptr<impl> pImpl;
};

#endif /* _IMPL_H */
