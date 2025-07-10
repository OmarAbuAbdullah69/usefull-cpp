
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
namespace zm {
	std::string loadFile(const char *path) {
		std::ifstream file(path, std::ios::ate|std::ios::app|std::ios::binary);
		if(!file.is_open()) throw std::runtime_error("faild to open file\n");
		int size = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);
		char buf[size+1];
		file.read(buf, size);
		buf[size] = '\0';
		return std::string(buf);
	}
}

int main (int argc, char *argv[]) {
	std::cout << zm::loadFile("main.cpp") << std::endl;
	return 0;
}
