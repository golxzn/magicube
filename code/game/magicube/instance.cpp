#include <optional>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>  // translate, rotate
#include <glm/ext/matrix_clip_space.hpp> // perspective

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>
#include <glbinding/gl/gl.h>

#include <core/app/application.hpp>
#include <core/io/inputs.hpp>

#include "game/magicube/instance.hpp"

namespace gzn::game::magicube {

void instance::start() {
	// == == == == == == == == == == SHADERS SOURCES == == == == == == == == == == //

	static constexpr std::string_view vertex_shader_text{ R"glsl(
		#version 440 core

		layout (location = 0) in vec3 position;
		layout (location = 1) in vec3 color;

		out vec3 fragment_color;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(position, 1.0);

			fragment_color = color;
		};
	)glsl" };

	static constexpr std::string_view fragment_shader_text{ R"glsl(
		#version 440 core

		in vec3 fragment_color;
		out vec4 FragColor;

		void main() {
			FragColor = vec4(fragment_color, 1.0);
		}
	)glsl" };

	// == == == == == == == == == == SHADERS PROGRAM == == == == == == == == == == //

	static const auto make_shader{ [](const auto source_code, const auto type) {
		const uint32_t shader{ gl::glCreateShader(type) };
		const auto *text{ source_code.data() };
		const auto text_length{ static_cast<gl::GLint>(source_code.length()) };
		gl::glShaderSource(shader, 1, &text, &text_length);
		gl::glCompileShader(shader);
		int32_t success{};
		gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &success);
		if (!success) {
			char info_log[512];
			gl::glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
			spdlog::error("[instance::start] Failed to compile '{}' shader: {}",
				magic_enum::enum_name(type), info_log);
			return std::optional<uint32_t>{};
		}
		return std::make_optional(shader);
	}};

	const auto vertex_shader{ make_shader(vertex_shader_text, gl::GL_VERTEX_SHADER) };
	const auto fragment_shader{ make_shader(fragment_shader_text, gl::GL_FRAGMENT_SHADER) };

	if (!vertex_shader || !fragment_shader) {
		return;
	}

	program = gl::glCreateProgram();
	gl::glAttachShader(program, vertex_shader.value());
	gl::glAttachShader(program, fragment_shader.value());
	gl::glLinkProgram(program);
	int32_t success{};
	gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[512];
		gl::glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
		spdlog::error("[instance::start] Failed to link program: {}", info_log);
		return;
	}

	gl::glDeleteShader(vertex_shader.value());
	gl::glDeleteShader(fragment_shader.value());

	// == == == == == == == == == == ==  BUFFERS  == == == == == == == == == == == //

	constexpr float vertices[] = {
		// positions          // color
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f  // top left
	};
	constexpr uint32_t indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	gl::glGenVertexArrays(1, &VAO);
	gl::glGenBuffers(1, &VBO);
	gl::glGenBuffers(1, &EBO);

	gl::glBindVertexArray(VAO);

	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, VBO);
	gl::glBufferData(gl::GL_ARRAY_BUFFER, sizeof(vertices), vertices, gl::GL_STATIC_DRAW);

	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, EBO);
	gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, gl::GL_STATIC_DRAW);

	// position attribute
	const int32_t vpos_location{ gl::glGetAttribLocation(program, "position") };
	gl::glVertexAttribPointer(vpos_location, 3, gl::GL_FLOAT, gl::GL_FALSE, 6 * sizeof(float), (void*)0);
	gl::glEnableVertexAttribArray(vpos_location);

	// color attribute
	const int32_t vcol_location{ gl::glGetAttribLocation(program, "color") };
	gl::glVertexAttribPointer(vcol_location, 3, gl::GL_FLOAT, gl::GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	gl::glEnableVertexAttribArray(vcol_location);

	// == == == == == == == == == == PROGRAM UNIFORM == == == == == == == == == == //

	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = make_projection();

	set_matrix("model", model);
	set_matrix("view", view);
	set_matrix("projection", projection);
}

void instance::update(const double delta) {
	if (core::io::inputs::just_released(core::io::key::escape)) {
		paused = !paused;
	}

	if (paused) return;

	static double timer{ 0.0 };
	timer += delta;

	const double offset{ glm::sin(timer * 5.0) * 0.01 };
	const double rotation{ delta * glm::radians(180.0) };

	model = glm::rotate(model, static_cast<float>(rotation), glm::vec3{ 0.0f, 1.0f, 0.0f }),
	model = glm::translate(model, glm::vec3{ 0.0f, offset, 0.0f });
}

void instance::draw() {
	gl::glUseProgram(program);
	set_matrix("model", model);
	set_matrix("view", view);
	set_matrix("projection", projection);

	gl::glBindVertexArray(VAO);
	gl::glDrawElements(gl::GL_TRIANGLES, 6, gl::GL_UNSIGNED_INT, 0);
}

void instance::stop() {
	gl::glDeleteProgram(program);

	gl::glDeleteBuffers(1, &EBO);
	gl::glDeleteBuffers(1, &VBO);
	gl::glDeleteVertexArrays(1, &VAO);
}

void instance::pause() { paused = true; }
void instance::resume() { paused = false; }

void instance::notification(const core::notification_type type) {
	switch (type) {
		case core::notification_type::focus_gained:
			resume();
			break;

		case core::notification_type::focus_lost:
			pause();
			break;

		case core::notification_type::framebuffer_size_changed:
			projection = make_projection();
			break;

		default: break;
	}
}

void instance::set_matrix(const std::string_view name, const glm::mat4x4 &value) {
	const auto location{ gl::glGetUniformLocation(program, name.data()) };
	gl::glUniformMatrix4fv(location, 1, gl::GL_FALSE, glm::value_ptr(value));
}

glm::mat4x4 instance::make_projection() {
	auto window{ glfwGetCurrentContext() };
	int32_t width{ 0 };
	int32_t height{ 0 };
	glfwGetWindowSize(window, &width, &height);
	const float aspect_ratio{ static_cast<float>(width) / static_cast<float>(height) };
	return glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
}


} // namespace gzn::game::magicube
