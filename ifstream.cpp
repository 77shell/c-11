#include <iostream>
#include <fstream>
#include <string>
#include <atomic>
#include <memory>

using namespace std;


std::ifstream::pos_type
flen(ifstream &f)
{
	f.seekg(0, ios::end);
	std::ifstream::pos_type len {f.tellg()};
	f.seekg(0, ios::beg);
	return len;
}

std::pair<std::shared_ptr<char[]>, std::size_t>
open_file(std::string path)
{
	ifstream stream {path, ios::in | ios::binary};
	if (stream.is_open()) {
		cout << "File is opened: " << path << endl;
		// int t;
		// stream >> t;
		// cout << path << "\t" << t << endl;

		std::ifstream::pos_type len {flen(stream)};
		std::shared_ptr<char[]> data_buf {new char[len]};
		stream.read(data_buf.get(), len);
		return make_pair<std::shared_ptr<char[]>, std::size_t>(std::move(data_buf), len);
	}
	else {
		cout << "Cannot open " << path << endl;
	}

	stream.close();
	throw;
}

void
do_nothing(std::shared_ptr<char[]> ptr)
{
	cout << __func__ << " shared_ptr count: " << ptr.use_count() << endl;
}

int main(int argc, char *argv[])
{
	const char *path {"/sys/class/thermal/thermal_zone1/temp"};
	if (argc > 1) {
		printf("Arg-1: %s\n", argv[1]);
		path = argv[1];
	}

	std::pair<std::shared_ptr<char[]>, std::size_t> filebuf {open_file(path)};
	for (int i {}; i < 256; i) {
		fprintf(stderr, "%02x ", filebuf.first[i++]);
		if (i % 16 == 0)
			cout << endl;
	}
	cout << __func__ << " shared_ptr count: " << filebuf.first.use_count() << endl;
	do_nothing(filebuf.first);
	cout << __func__ << " shared_ptr count: " << filebuf.first.use_count() << endl;
	cout << "File length: " << filebuf.second << endl;

	cout << "Release buffer\n";
	filebuf.first.reset();
	cout << __func__ << " shared_ptr count: " << filebuf.first.use_count() << endl;
}
