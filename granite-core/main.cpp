#include <iostream>
#include "src/graphics/window.h"
#include "src/graphics/shader.h"
#include "src/utils/io.h"
#include "src/utils/file.h"
#include "src/math/math.h"
#include "src/graphics/buffers/indexbuffer.h"
#include "src/graphics/buffers/vertexarray.h"


int main() {
	 
	using namespace granite;
	using namespace graphics;
	using namespace math;
	using namespace std;

	Window window("Granite", 800, 600);

	const std::vector<GLfloat>vertices = {
		0, 0, 0,
		0, 3, 0,
		8, 3, 0,
		8, 0, 0
	};
	const std::vector<GLushort> indices = {
		0, 1, 2,
		2, 3, 0
	};
	std::shared_ptr<Buffer> vbo = std::make_shared<Buffer>(vertices, 3);
	IndexBuffer ibo(indices, 6);

	VertexArray vao;
	vao.addBuffer(vbo, 0);

	mat4 ortho = mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);

	Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");
	shader.enable();
	shader.setUniformMat4("pr_matrix", ortho);
	shader.setUniformMat4("vw_matrix", mat4::translation(vec3(2, 2, 0)));
	shader.setUniformMat4("ml_matrix", mat4::rotation(10, vec3(0, 0, 1)));

	while (!window.closed()) {
		window.clear();
		shader.setUniformMat4("ml_matrix", mat4::rotation(window.getCursorX(), vec3(0, 0, 1)));
		vao.bind();
		ibo.bind();
		glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_SHORT, 0);

		/*
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL error: " << err << std::endl;
		}
		*/

		vao.unbind();
		ibo.unbind();
		window.update();

	}

	return 0;
}