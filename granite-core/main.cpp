#include <iostream>
#include "src/graphics/window.h"
#include "src/utils/io.h"
#include "src/math/math.h"


int main() {
	 
	using namespace granite;
	using namespace graphics;
	using namespace math;
	using namespace std;

	Window window("Granite", 800, 600);

	system("PAUSE");
	return 0;


	while (!window.closed()) {
		window.update();
		//printl(window.getKeyPressed());
		printl(window.getCursorX());
	}

	return 0;
}