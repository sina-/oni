#include <iostream>
#include "src/graphics/window.h"
#include "src/graphics/shader.h"
#include "src/utils/io.h"
#include "src/utils/file.h"
#include "src/math/math.h"
#include "src/graphics/buffers/indexbuffer.h"
#include "src/graphics/buffers/vertexarray.h"
#include "src/graphics/renderable2d.h"
#include "src/graphics/simple2drenderer.h"
#include "src/graphics/renderer2d.h"


int main() {
	 
	using namespace granite;
	using namespace graphics;
	using namespace math;
	using namespace std;

	Window window("Granite", 800, 600);

	Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");
	shader.enable();
	shader.setUniformMat4("pr_matrix", mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f));
	shader.setUniformMat4("vw_matrix", mat4::translation(vec3(2, 2, 0)));

	Renderable2D sprite1(math::vec2(4, 4), math::vec3(5, 5, 0), math::vec4(1, 0, 1, 1), shader);
	Renderable2D sprite2(math::vec2(1, 2), math::vec3(0, 1, 1), math::vec4(0, 1, 1, 1), shader);

	Simple2DRenderer renderer;

	while (!window.closed()) {
		window.clear();

		/*
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL error: " << err << std::endl;
		}
		*/
		
		renderer.submit(&sprite1);
		renderer.submit(&sprite2);
		renderer.flush();
		window.update();

	}

	return 0;
}