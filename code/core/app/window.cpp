#include <algorithm>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "core/app/window.hpp"
#include "core/io/inputs.hpp"

namespace gzn::core {

window::window(const glm::i32vec2 &size, const std::string_view title, const bool full_screen)
	: m_title{ title } {

	static const auto set_hint{ [] (const auto hint, const auto value, const auto prompt) {
		glfwWindowHint(hint, value);
		spdlog::info("[window::window] [hint] {}: {}", prompt, value);
	} };

	auto monitor{ glfwGetPrimaryMonitor() };
	const auto *mode{ glfwGetVideoMode(monitor) };

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
#endif // defined(MAGICUBE_DEBUG)

	m_handle = full_screen
		? glfwCreateWindow(mode->width, mode->height, title.data(), monitor, nullptr)
		: glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);
	if (!m_handle) {
		spdlog::error("[window::window] Failed to create GLFW window");
		return;
	}

	glfwSetWindowUserPointer(m_handle, this);
	glfwSetWindowCloseCallback(m_handle,     &window::on_close);
	glfwSetWindowFocusCallback(m_handle,     &window::on_focus_changed);
	glfwSetFramebufferSizeCallback(m_handle, &window::on_framebuffer_size_changed);
	glfwSetDropCallback(m_handle,            &window::on_drop);

	glfwMakeContextCurrent(m_handle);

	io::inputs::assign(*this);
}

window::~window() {
	if (!valid()) return;

	io::inputs::reset(*this);

	if (m_listener && m_listener->has_additional_event<listener::additional_event::destroying>()) {
		m_listener->on_destroying();
	}

	glfwSetWindowCloseCallback(m_handle, nullptr);
	glfwSetWindowFocusCallback(m_handle, nullptr);
	glfwSetFramebufferSizeCallback(m_handle, nullptr);
	glfwSetDropCallback(m_handle, nullptr);
	glfwSetWindowUserPointer(m_handle, nullptr);

	glfwDestroyWindow(m_handle);
}

window::window(window &&other) noexcept
	: m_handle{ std::exchange(other.m_handle, nullptr) }
	, m_listener{ std::exchange(other.m_listener, nullptr) }
	, m_title{ std::move(other.m_title) } {

	if (m_handle) glfwSetWindowUserPointer(m_handle, this);
}

window &window::operator=(window &&other) noexcept {
	m_handle = std::exchange(other.m_handle, nullptr);
	m_listener = std::exchange(other.m_listener, nullptr);
	m_title = std::move(other.m_title);

	if (m_handle) glfwSetWindowUserPointer(m_handle, this);

	return *this;
}

void window::set_listener(listener &listener) noexcept {
	m_listener = &listener;
}

void window::reset_listener() noexcept {
	m_listener = nullptr;
}

void window::close() const noexcept {
	glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
}

void window::poll_events() const noexcept {
	glfwPollEvents();
}

void window::swap_buffers() const noexcept {
	glfwSwapBuffers(m_handle);
}

void window::set_title(const std::string_view title) noexcept {
	glfwSetWindowTitle(m_handle, title.data());
	m_title = title;
}

bool window::valid() const noexcept {
	return m_handle != nullptr;
}

glm::i32vec2 window::size() const noexcept {
	if (!valid()) return { 0, 0 };

	glm::i32vec2 size;
	glfwGetWindowSize(m_handle, &size.x, &size.y);
	return size;
}

std::string_view window::title() const noexcept {
	return m_title;
}

bool window::should_close() const noexcept {
	return glfwWindowShouldClose(m_handle) == GLFW_TRUE;
}

window *window::load_myself(GLFWwindow *handle) noexcept {
	if (handle) {
		return reinterpret_cast<window *>(glfwGetWindowUserPointer(handle));
	}
	return nullptr;
}

void window::on_close(GLFWwindow *handle) {
	auto self{ load_myself(handle) };
	if (self == nullptr) return;

	auto list{ self->m_listener };
	if (list && list->has_additional_event<window::listener::additional_event::destroying>()) {
		list->on_destroying();
	}
}

void window::on_focus_changed(GLFWwindow *handle, const int32_t focused) {
	auto self{ load_myself(handle) };
	if (self == nullptr) return;

	if (focused == GLFW_TRUE) {
		io::inputs::assign(*self);
	} else {
		io::inputs::reset(*self);
	}
	if (auto list{ self->m_listener }; list) {
		list->on_focus_changed(focused == GLFW_TRUE);
	}
}

void window::on_framebuffer_size_changed(GLFWwindow *handle, const int32_t width, const int32_t height) {
	auto self{ load_myself(handle) };
	if (self == nullptr) return;

	if (auto list{ self->m_listener }; list) {
		list->on_framebuffer_size_changed({ width, height });
	}
}

void window::on_drop(GLFWwindow *handle, const int32_t count, const char **paths) {
	auto self{ load_myself(handle) };
	if (self == nullptr) return;

	if (auto list{ self->m_listener }; list) {
		std::vector<std::string_view> paths_list;
		paths_list.reserve(count);

		std::transform(paths, paths + count, std::back_inserter(paths_list),
			[] (const auto path) { return std::string_view{ path }; });

		list->on_drop(std::move(paths_list));
	}
}



} // namespace gzn::core
