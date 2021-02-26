/*
The MIT License (MIT)

Copyright (c) 2021 James Boer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if defined(__APPLE__) && defined(__MACH__)
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#endif

#include <linmath.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>


/*! \namespace */
namespace ImFrame
{

	typedef struct Vertex
	{
		vec2 pos;
		vec3 col;
	} Vertex;

	static const Vertex vertices[3] =
	{
		{ { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
		{ {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
		{ {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
	};

	static const char * vertex_shader_text =
		"#version 330\n"
		"uniform mat4 MVP;\n"
		"in vec3 vCol;\n"
		"in vec2 vPos;\n"
		"out vec3 color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
		"    color = vCol;\n"
		"}\n";

	static const char * fragment_shader_text =
		"#version 330\n"
		"in vec3 color;\n"
		"out vec4 fragment;\n"
		"void main()\n"
		"{\n"
		"    fragment = vec4(color, 1.0);\n"
		"}\n";

	static void error_callback([[maybe_unused]] int error, const char * description)
	{
		fprintf(stderr, "Error: %s\n", description);
	}

	static void key_callback(GLFWwindow * window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

    void RunImFrame()
    {
		glfwSetErrorCallback(error_callback);

		if (!glfwInit())
			exit(EXIT_FAILURE);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GLFWwindow * window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwSetKeyCallback(window, key_callback);

		glfwMakeContextCurrent(window);
		gladLoadGL();
		glfwSwapInterval(1);

		// NOTE: OpenGL error checks have been omitted for brevity

		GLuint vertex_buffer;
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
		glCompileShader(vertex_shader);

		const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
		glCompileShader(fragment_shader);

		const GLuint program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		const GLint mvp_location = glGetUniformLocation(program, "MVP");
		const GLint vpos_location = glGetAttribLocation(program, "vPos");
		const GLint vcol_location = glGetAttribLocation(program, "vCol");

		GLuint vertex_array;
		glGenVertexArrays(1, &vertex_array);
		glBindVertexArray(vertex_array);
		glEnableVertexAttribArray(vpos_location);
		glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void *)offsetof(Vertex, pos));
		glEnableVertexAttribArray(vcol_location);
		glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void *)offsetof(Vertex, col));

		while (!glfwWindowShouldClose(window))
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			const float ratio = width / (float)height;

			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT);

			mat4x4 m, p, mvp;
			mat4x4_identity(m);
			mat4x4_rotate_Z(m, m, (float)glfwGetTime());
			mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
			mat4x4_mul(mvp, p, m);

			glUseProgram(program);
			glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);
			glBindVertexArray(vertex_array);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwDestroyWindow(window);

		glfwTerminate();
		exit(EXIT_SUCCESS);
    }

}
