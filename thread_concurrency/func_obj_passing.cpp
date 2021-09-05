#include <thread>
#include <future>
#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>

std::condition_variable cv;
std::mutex mutex;

enum Signal_e {
        eNone,
        eRisingEdge,
        eFallingEdge,
        ePositive_Pulse,
        eNegative_Pulse
};

Signal_e S;

class BUTTON {
public:
        template<class Callable>
        BUTTON(Callable &&func_obj)
                {
                        auto fu = std::async(std::launch::async, func_obj);
                        //fu.get();
                }
};


class FUNCobj {
public:
        FUNCobj()
                : mesg {"Hello"}
                {
                        std::cout << __func__ << " Constructor\n";
                }
        
        FUNCobj(std::string s)
                : mesg {s}
                {
                        std::cout << __func__ << " Constructor\n";
                }
        
        void operator()() {
                std::cout << mesg << std::endl;
        }

        std::string mesg;
};


void
Detect_signals()
{
        //auto f = std::async(FUNCobj());
        //f.get();
        //FUNCobj fobj2 {"I Love You"};
        //BUTTON b1 {FUNCobj()};
        //BUTTON b1 ([](){std::cout << "Hello\n";});
        //BUTTON b2 ([](){std::cout << "I love you\n";});
        FUNCobj fobj3;
        BUTTON b3 {fobj3};

        BUTTON b4 {FUNCobj {}};
        BUTTON b5 {FUNCobj {"I love you"}};
        
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        for(int i=0;;)
        {
                {
                        std::unique_lock<std::mutex> lk {mutex};
                        S = eRisingEdge;
                }
                cv.notify_one();
                std::this_thread::sleep_for(std::chrono::seconds(3));
        }
}



int main(int argc, char *argv[])
{
        std::thread th_store {Detect_signals};
        
        for(;;)
        {
                Signal_e tmp;
                {
                        std::unique_lock<std::mutex> lk {mutex};
                        cv.wait(lk, [] { return S != eNone; });
                        tmp = S;
                        S = eNone;
                }
                
                switch(tmp) {
                case eNone:           std::cout << "eNone\n"; break;
                case eRisingEdge:     std::cout << "eRisingEdge\n"; break;
                case eFallingEdge:    std::cout << "eFallingEdge\n";    break;
                case ePositive_Pulse: std::cout << "ePositive_Pulse\n"; break;
                case eNegative_Pulse: std::cout << "eNegative_Pulse\n"; break;
                }
                
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        th_store.join();
        return 0;
}
