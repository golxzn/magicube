#include <iostream>

#include <core/temp.hpp>
#include <game/temp.hpp>

int main(int argc, char *argv[]) {
	std::cout << gzn::magicube::core::temp_function() << std::endl;
	std::cout << gzn::magicube::game::temp_function() << std::endl;
	return 0;
}