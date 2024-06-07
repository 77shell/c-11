#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

int main() {
    std::string filename = "sw_data.bin";
    std::ofstream outfile(filename, std::ios::binary);

    if (!outfile) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 1;
    }

    // Define the size of the random data to generate
    const std::size_t dataSize = 10240; // 10 KB data

    // Create a buffer to hold the random data
    std::vector<char> buffer(dataSize);
    auto ptr {buffer.data()};
    for (std::size_t i {}; i < dataSize; ++i)
	    *ptr++ = static_cast<char>(i);

    // Write the buffer to the file
    outfile.write(buffer.data(), buffer.size());

    if (!outfile) {
        std::cerr << "Failed to write data to file: " << filename << std::endl;
        return 1;
    }

    outfile.close();
    std::cout << "Successfully wrote " << dataSize << " bytes of random data to " << filename << std::endl;

    return 0;
}
