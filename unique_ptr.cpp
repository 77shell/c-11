#include <iostream>
#include <cassert>
#include <cstring>
#include <string>
#include <chrono>
#include <memory>

std::string
gen_timestamp()
{
	using namespace std::chrono;
	system_clock current;
	std::time_t t = current.to_time_t(current.now());
	char str_buf[32];
	std::strftime(str_buf, 32, "%F %T", std::localtime(&t));
	return std::string {str_buf};
}

int main(int argc, char *argv[])
{
        using namespace std;
        const size_t buf_size {96};
        const char *msg = "Hello everyone this is unique_ptr test";
        unsigned long long id {100};
        unique_ptr<char[]> buf = make_unique<char[]>(buf_size);
        for(int i=0; i<buf_size; i++)
                buf[i] = static_cast<unsigned char>(0);

        { // store data to buf
                *reinterpret_cast<unsigned long int*>(buf.get()) = id;
                char *buf_ptr {buf.get() + sizeof(id)};
                string date_time {gen_timestamp()};
                strcpy(buf_ptr, date_time.data());
                buf_ptr += date_time.length();
                buf_ptr++; /* null character */
                strcpy(buf_ptr, msg);
                buf_ptr += strlen(msg);
                buf_ptr++; /* null character */
                ptrdiff_t buf_len {buf_ptr - buf.get()};
        
                char *ptr {buf.get()};
                fprintf(stderr, "Occupied buffer len : %ld\n", buf_len);
                for(int i=0; i<buf_size; i++)
                        fprintf(stderr, "%02X ", buf[i]);

                cout << endl;
                for(int i=0; i<buf_size; i++)
                {
                        if(buf[i] == 0)
                                fprintf(stderr, "00 ");
                        else
                                fprintf(stderr, "%2c ", buf[i]);
                }
                cout << endl;
        }

        { // extract data from buf
                using namespace std;
                unsigned long int id;
                std::string datetime;
                std::string msg;

                char *ptr {buf.get()};
                id = *reinterpret_cast<unsigned long int*>(buf.get());
                ptr += sizeof(unsigned long int);
                fprintf(stderr, "ID: %ld\n", id);
                
                datetime = ptr;
                ptr += datetime.length();
                cout << "datetime len : " << datetime.length() << endl;
                ptr++; /* null character */
                cout << datetime << endl;
                msg = ptr;
                cout << msg << endl;
        }
        return 0;
}
