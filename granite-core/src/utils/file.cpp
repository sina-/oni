#include "file.h"

std::string granite::read_file(const char* path)
{
	std::ifstream ifs(path);
	std::string content(std::istreambuf_iterator<char>(ifs.rdbuf()), std::istreambuf_iterator<char>());
	if (content.size() == 0)
		throw std::runtime_error("Empty file " + std::string(path));
	return content;
}
