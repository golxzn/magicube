#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>

#include "core/app/application.hpp"

namespace gzn::core {

std::unique_ptr<application> application::create(const glm::ivec2 &size, const std::string_view title) {
	glfwSetErrorCallback(&application::on_error);

	if (glfwInit() == GLFW_FALSE) {
		spdlog::error("[application::create] GLFW initialization failed");
		return nullptr;
	}

	if (auto window{ glfwGetCurrentContext() }; window && glfwGetWindowUserPointer(window)) {
		spdlog::error("[application::create] Application is already running");
		return nullptr;
	}

	if (std::unique_ptr<application> app{ new application{ size, title } }; app && app->initialized()) {
		return std::move(app);
	}
	return nullptr;
}

application::~application() {
	if (initialized()) {
		auto window{ glfwGetCurrentContext() };
		glfwSetWindowUserPointer(window, nullptr);
		glfwSetWindowFocusCallback(window, nullptr);
		glfwSetFramebufferSizeCallback(window, nullptr);
		glfwSetDropCallback(window, nullptr);
	}

	glfwTerminate();
}

bool application::initialized() const noexcept {
	auto window{ glfwGetCurrentContext() };
	return window && glfwGetWindowUserPointer(window) == this;
}

application::application(const glm::ivec2 &size, const std::string_view title) {
	auto monitor{ glfwGetPrimaryMonitor() };
	const auto *mode{ glfwGetVideoMode(monitor) };

	static const auto set_hint{ [] (const auto hint, const auto value, const auto prompt) {
		glfwWindowHint(hint, value);
		spdlog::info("[application::application] [hint] {}: {}", prompt, value);
	} };

	set_hint(GLFW_CONTEXT_VERSION_MAJOR, defaults::opengl::version_major, "OpenGL major version");
	set_hint(GLFW_CONTEXT_VERSION_MINOR, defaults::opengl::version_minor, "OpenGL minor version");
	set_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE, "OpenGL profile");
	set_hint(GLFW_DOUBLEBUFFER, GLFW_TRUE, "Double buffering");

	set_hint(GLFW_RED_BITS,     mode->redBits,     "[video mode] red bits    ");
	set_hint(GLFW_GREEN_BITS,   mode->greenBits,   "[video mode] green bits  ");
	set_hint(GLFW_BLUE_BITS,    mode->blueBits,    "[video mode] blue bits   ");
	set_hint(GLFW_REFRESH_RATE, mode->refreshRate, "[video mode] refresh rate");

#if defined(MAGICUBE_DEBUG)
	set_hint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE, "Debug context");
	monitor = nullptr;
#endif // defined(MAGICUBE_DEBUG)


	auto window{ glfwCreateWindow(size.x, size.y, title.data(), monitor, nullptr) };
	if (!window) {
		spdlog::error("[application::create] Failed to create GLFW window");
		return;
	}

	glfwMakeContextCurrent(window);

	glfwSetWindowUserPointer(window, this);
	glfwSetWindowFocusCallback(window,     &application::on_focus_changed);
	glfwSetFramebufferSizeCallback(window, &application::on_framebuffer_size_changed);
	glfwSetDropCallback(window,            &application::on_drop);


	glbinding::initialize(glfwGetProcAddress);
	gl::glEnable(gl::GL_DEPTH_TEST);
}

auto application::run() -> exit_code_type {
	if (!ready()) {
		spdlog::warn("[application::run] Application is not ready");
		return EXIT_SUCCESS;
	}

	gl::glClearColor(
		defaults::opengl::clear_color::r,
		defaults::opengl::clear_color::g,
		defaults::opengl::clear_color::b,
		defaults::opengl::clear_color::a
	);

	double delta{ 0.0 };
	double previous{ 0.0 };
	double current{ 0.0 };

	game->start();

	auto window{ glfwGetCurrentContext() };

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			notify(notification_type::quit);
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		current = glfwGetTime();
		delta = current - previous;
		previous = current;

		game->update(delta);

		gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

		game->draw(/* render */);

		glfwSwapBuffers(window);
	}

	game->stop();

	return EXIT_SUCCESS;
}

void application::assign_game(const std::shared_ptr<game_base> &game) {
	if (this->game == nullptr) {
		this->game = game;
		spdlog::info("[application::assign_game] Game was assigned");
	} else {
		spdlog::warn("[application::assign_game] Game is already assigned. Ignoring upcoming game");
	}
}

void application::notify(const notification_type notification) const {
	if (game != nullptr) {
		game->notification(notification);
	}
}

void application::on_focus_changed(GLFWwindow *window, const int32_t focused) {
	spdlog::info("[application::on_focus_changed] Window {}", focused ? "got focus" : "lost focus");
	if (auto app{ static_cast<application *>(glfwGetWindowUserPointer(window)) }; app != nullptr) {
		app->notify(focused == GLFW_TRUE
			? notification_type::focus_gained
			: notification_type::focus_lost
		);
	}
}

void application::on_framebuffer_size_changed(GLFWwindow *window, const int32_t width,
		const int32_t height) {
	gl::glViewport(0, 0, width, height);
	if (auto app{ static_cast<application *>(glfwGetWindowUserPointer(window)) }; app != nullptr) {
		app->notify(notification_type::framebuffer_size_changed);
	}
}

void application::on_drop(GLFWwindow *window, const int32_t count, const char **paths) {
	spdlog::info("[application::on_drop] Dropped {} path{}", count, count > 1 ? "s" : "");
	for (int32_t i = 0; i < count; i++) {
		spdlog::info("[application::on_drop]    Path #{:<3}: {}", i, paths[i]);
	}
}

void application::on_error(const int32_t code, const char *message) {
	spdlog::error("[glfw] Error #{:X}: '{}'", code, message);
}

} // namespace gzn::core
