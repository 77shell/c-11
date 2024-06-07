#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

// using namespace util;
using namespace std;

std::string
exec(std::string cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void
set_uboot_var(const char *var, const char *val)
{
	constexpr size_t bufsize {128};
	std::array<char, bufsize> cmdbuf;
	sprintf(cmdbuf.data(), "fw_setenv %s %s", var, val);
	exec(cmdbuf.data());
}

std::string
get_uboot_var(const char *var)
{
	constexpr size_t bufsize {128};
	std::array<char, bufsize> cmdbuf;
	sprintf(cmdbuf.data(), "fw_printenv | sed -ne 's/%s=//p' | tr -d '\n'", var);
	return exec(cmdbuf.data());
}

void
write_module_sn(const char *sn)
{
	// Disable wirte protect on mmcblk1boot1
	exec("echo 0 > /sys/block/mmcblk1boot1/force_ro");

	// Write U-Boot variable
	// Variable name: ctrl_mod_serial_nbr
	set_uboot_var("ctrl_mod_serial_nbr", sn);

	// Enable wirte protect on mmcblk1boot1
	exec("echo 1 > /sys/block/mmcblk1boot1/force_ro");
}

int
main(int argc, char *argv[])
{
	write_module_sn("Are You Happy!");
	string date {get_uboot_var("ctrl_date")};
	string time {get_uboot_var("ctrl_time")};
	cout << "Production Date/Time: " << date << " " << time << endl;

	string mod_sn {get_uboot_var("ctrl_mod_serial_nbr")};
	cout << "Module SN: " << mod_sn << endl;

	string pcb_sn {get_uboot_var("ctrl_serial_nbr")};
	cout << "PCB SN: " << pcb_sn << endl;

	string part_nbr {get_uboot_var("ctrl_part_nbr")};
	cout << "Part Number: " << part_nbr << endl;

	// using namespace std;
	
	// string date {exec("fw_printenv | sed -ne 's/ctrl_date=//p' | tr -d '\n'")};
	// string time {exec("fw_printenv | sed -ne 's/ctrl_time=//p' | tr -d '\n'")};
	// cout << "Production Date/Time: " << date << " " << time << endl;

	// string mod_sn {exec("fw_printenv | sed -ne 's/ctrl_mod_serial_nbr=//p' | tr -d '\n'")};
	// cout << "Module SN: " << mod_sn << endl;

	// string pcb_sn {exec("fw_printenv | sed -ne 's/ctrl_serial_nbr=//p' | tr -d '\n'")};
	// cout << "PCB SN: " << pcb_sn << endl;

	// string part_nbr {exec("fw_printenv | sed -ne 's/ctrl_part_nbr=//p' | tr -d '\n'")};
	// cout << "Part Number: " << part_nbr << endl;
}


// #include <cstdio>
// #include <iostream>
// #include <memory>
// #include <stdexcept>
// #include <string>
// #include <array>

// std::string
// exec(std::string cmd)
// {
//     std::array<char, 128> buffer;
//     std::string result;
//     std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
//     if (!pipe) {
//         throw std::runtime_error("popen() failed!");
//     }
//     while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
//         result += buffer.data();
//     }
//     return result;
// }

// void
// write_module_sn(std::string sn)
// {
// 	using namespace std;
// 	/** Set U-Boot variable
// 	 */
// 	// Disable wirte protect on mmcblk1boot1
// 	exec("echo 0 > /sys/block/mmcblk1boot1/force_ro");

// 	// Write U-Boot variable
// 	// Variable name: ctrl_mod_serial_nbr
// 	string cmd {"fw_setenv ctrl_mod_serial_nbr "};
// 	cmd += sn;
// 	exec(cmd);

// 	// Enable wirte protect on mmcblk1boot1
// 	exec("echo 1 > /sys/block/mmcblk1boot1/force_ro");

// 	// Read variable
// 	string mod_sn {exec("fw_printenv | sed -ne 's/ctrl_mod_serial_nbr//p' | tr -d '\n'")};
// 	cout << "Module SN: " << mod_sn << endl;
// }

// int
// main(int argc, char *argv[])
// {
// 	using namespace std;
	
// 	string date {exec("fw_printenv | sed -ne 's/ctrl_date=//p' | tr -d '\n'")};
// 	string time {exec("fw_printenv | sed -ne 's/ctrl_time=//p' | tr -d '\n'")};
// 	cout << "Production Date/Time: " << date << " " << time << endl;

// 	string mod_sn {exec("fw_printenv | sed -ne 's/ctrl_mod_serial_nbr=//p' | tr -d '\n'")};
// 	cout << "Module SN: " << mod_sn << endl;

// 	string pcb_sn {exec("fw_printenv | sed -ne 's/ctrl_serial_nbr=//p' | tr -d '\n'")};
// 	cout << "PCB SN: " << pcb_sn << endl;

// 	string part_nbr {exec("fw_printenv | sed -ne 's/ctrl_part_nbr=//p' | tr -d '\n'")};
// 	cout << "Part Number: " << part_nbr << endl;
// }
