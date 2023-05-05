#include <iostream>
#include <sstream>
#include <vector>
#include <map>


std::map<std::string, std::string> map {
	{ "MAN", "Manufacturer" },
	{ "P/N", "Part number" },
	{ "S/N", "Serial number" },
	{ "REV", "Hardware revision" },
	{ "DAT", "Production date" },
	{ "SW",  "SW Version" }
};

static void
parser(std::istringstream &&input)
{
	std::vector<std::string> vec;
 
	std::cout << input.str() << std::endl;
	for (std::string s; std::getline(input, s, '#');)
		vec.push_back(s);

	for (std::vector<std::string>::iterator it {vec.begin()}; it != vec.end(); it++) {
		auto r {map.find(*it)};
		if (r != map.end()) {
			it++;
			std::cout << r->second << ": " << *it << std::endl;;
		}
	}
}

int main()
{
	std::istringstream input("MAN#EXICOM#P/N#20160225+#S/N#CCBBAA998877665544332211#REV#123#SW#6.60B01#");
	parser(std::move(input));
	std::cout << '\n';
	input = std::istringstream("#MAN#EXICOM2#P/N#20160225+2#S/N#CCBBAA998877665544332211A#REV#123#DAT#3419#SW#6.60B02#");
	parser(std::move(input));
	return 0;
}
