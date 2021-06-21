/*****************************************************************
 *  Copyright (c) 2019-2020 Delta Products
 *
 *  THIS IS UNPUBLISHED PROPRIETARY TRADE SECRET SOURCE CODE OF
 *  Delta Products
 *
 *  The copyright above and this notice must be preserved in all copies of this
 *  source code.  The copyright notice above does not evidence any actual or
 *  intended publication of such source code.  This source code may not be
 *  copied, disclosed, distributed, demonstrated or licensed except as expressly
 *  authorized by Delta Products.
 *************************************************************/
#include <map>
#include <thread>
#include <condition_variable>
#include <vector>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

#define __MESG_STATE_MACH

#ifdef __MESG_STATE_MACH
#define MESG(str)   fprintf(stderr, "%s\n", str)
#define dprintf     printf
#else
#define MESG(str)
#define dprintf
#endif

namespace {

	struct Message_t {
		enum Event_e {
			eEvent_,
			eEvent_Start,
			eEvent_Stop,
			eEvent_Stop_CoolantTemp_High,
			eEvent_Stop_OilPressure_Low,
			eEvent_Terminate
		};

		Message_t()
			: event {eEvent_}
			{}

		template<class Rep, class Period>
		bool wait_for(Event_e listen_evt, std::chrono::duration<Rep,Period> &&wait)
			{
				std::unique_lock<std::mutex> lck {mtx};
				return cv.wait_for(lck, wait,
						   [&]{
							   if(event == eEvent_Terminate)
								   throw eEvent_Terminate;

							   bool r {event == listen_evt};
							   if(r)
								   event = eEvent_;
							   return r;
						   }
					);
			}

		void wait(Event_e listen_evt)
			{
				std::unique_lock<std::mutex> lck {mtx};
				cv.wait(lck, [&]{
						     if(event == eEvent_Terminate)
							     throw eEvent_Terminate;
#ifdef DEBUG
						     std::cout << __func__ << " Wake up (event : " << event << " listening : " << listen_evt << std::endl;
#endif
						     bool r {event == listen_evt};
						     if(r)
							     event = eEvent_;
						     return r;
					     }
					);
			}

		void wait(const std::vector<Event_e> &listen_evts)
			{
				std::unique_lock<std::mutex> lck {mtx};
				cv.wait(lck, [&]{
						     if(event == eEvent_Terminate)
							     throw eEvent_Terminate;

						     for(auto evt : listen_evts) {
							     if(event == evt) {
								     event = eEvent_;
								     return true;
							     }
						     }
						     return false;
					     }
					);
			}

		void notify(Event_e evt)
			{
				{
					std::lock_guard<std::mutex> lck {mtx};
                                        event = evt;
				}
				cv.notify_all();
				std::this_thread::yield();
			}
	private:
		Event_e event;
		std::mutex mtx;
		std::condition_variable cv;
	};

        Message_t Message;
}

void routine_a()
{
        std::cout << "Waiting: Start-event\n";
        Message.wait(Message_t::eEvent_Start);
}

void thread_a()
{
        for(;;)
	{
		try {
			//Message.wait_for(Message_t::eEvent_Terminate, 0ms);
			routine_a();
		}
		catch(Message_t::Event_e evt) {
			if(evt == Message_t::eEvent_Terminate) {
                                std::cout << __func__ << ": Catch termination\n";
				return;
			}
                        std::cout << __func__ << ": Catch a message\n";
		}
                std::cout << __func__ << std::endl;
		std::this_thread::sleep_for(1s);
        }
}


int main(int argc, char *argv[])
{
        std::thread th_a {thread_a};
        std::string cmd;

        for(;;)
        {
                std::cin >> cmd;
                if(cmd == "terminate") {
                        std::cout << "Got termination\n";
                        Message.notify(Message_t::eEvent_Terminate);
                        th_a.join();
                        break;
                }
                std::this_thread::sleep_for(1s);
        }
        std::cout << __func__ << " done\n";
}
