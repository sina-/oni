#include <iostream>
#include "graphics/window.h"
#include "graphics/shader.h"
#include "utils/io.h"
#include "utils/file.h"
#include "math/mat4.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "buffers/indexbuffer.h"
#include "buffers/vertexarray.h"
#include "graphics/renderable2d.h"
#include "graphics/simple2drenderer.h"
#include "graphics/renderer2d.h"


int main() {
	 
	using namespace granite;
	using namespace graphics;
	using namespace math;
	using namespace std;

	Window window("Granite", 800, 600);

	Shader shader("shaders/basic.vert", "shaders/basic.frag");
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