
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;

static void
print_date_time(std::tm &datetime)
{
        cout << "Year: " << datetime.tm_year << endl;
        cout << "Month: " << datetime.tm_mon + 1 << endl;
        cout << "Day: " << datetime.tm_mday << endl;
        cout << "Hour: " << datetime.tm_hour << endl;
        cout << "Min: " << datetime.tm_min << endl;
        cout << "Sec: " << datetime.tm_sec << endl;
}

int main(int argc, char *argv[])
{
        //std::time_t since_epoc_sec {std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
        std::time_t since_epoc_sec {std::time(nullptr)};
        cout << "Since epoc (seconds): " << since_epoc_sec << endl;

        print_date_time(*std::gmtime(&since_epoc_sec));
        cout << "Local date/time\n"
        print_date_time(*std::localtime(&since_epoc_sec));
}
