#include <oni-core/utils/io.h>
#include <iostream>


namespace oni {
	namespace utils {
		void printl(const std::string &text) {
			std::cout << text << std::endl;
		}

		void printl(int text) {
			std::cout << text << std::endl;
		}

		void printl(double text) {
			std::cout << text << std::endl;
		}

		void printl(float text) {
			std::cout << text << std::endl;
		}
	}
}
