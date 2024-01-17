#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>

#include "core/tools/clock.hpp"
#include "core/app/application.hpp"
#include "core/app/window.hpp"
#include "core/io/inputs.hpp"

namespace gzn::core {

std::unique_ptr<application> application::create() {
	glfwSetErrorCallback(&application::on_error);

	if (glfwInit() == GLFW_FALSE) {
		spdlog::error("[application::create] GLFW initialization failed");
		return nullptr;
	}

	if (s_instance_exists) {
		spdlog::error("[application::create] Application is already running");
		return nullptr;
	}

	if (std::unique_ptr<application> app{ new application{} }; app && app->initialized()) {
		s_instance_exists = true;
		return std::move(app);
	}
	return nullptr;
}

application::~application() {
	m_window.reset(); // to ensure that window will be destroyed before glfwTerminate
	glfwTerminate();
}

bool application::initialized() const noexcept {
	return m_window && m_window->valid();
}

application::application() : m_window{ std::make_unique<window>() } {
	if (!m_window->valid()) {
		return;
	}

	m_window->set_listener(*this);
	listen_to_additional_event<window::listener::additional_event::drop>();

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
	constexpr std::string_view title_format{ "[{:>5} FPS] {}" };

	m_game->start();

	tools::clock<double> clock;
	while (!m_window->should_close()) {
		m_window->poll_events();

		if (io::inputs::just_released<io::modifier::control>(io::key::q)) {
			notify(notification_type::quit);
			m_window->close();
		}

		const auto delta_time{ clock.delta() };
		m_window->set_title(fmt::format(
			title_format, static_cast<int32_t>(1.0 / delta_time), defaults::window::title));
		m_game->update(delta_time);

		gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

		m_game->draw(/* render */);

		m_window->swap_buffers();
	}

	m_game->stop();

	return EXIT_SUCCESS;
}

void application::assign_game(const std::shared_ptr<game_base> &game) {
	if (m_game == nullptr) {
		m_game = game;
		spdlog::info("[application::assign_game] Game was assigned");
	} else {
		spdlog::warn("[application::assign_game] Game is already assigned. Ignoring upcoming game");
	}
}

void application::notify(const notification_type notification) const {
	if (m_game != nullptr) {
		m_game->notification(notification);
	}
}

void application::on_close() {
	spdlog::info("[application::on_close] Window is about to be closed");
	notify(notification_type::closing);
}

void application::on_focus_changed(const bool focused) {
	spdlog::info("[application::on_focus_changed] Window {}", focused ? "got focus" : "lost focus");
	notify(focused ? notification_type::focus_gained : notification_type::focus_lost);
}

void application::on_framebuffer_size_changed(const glm::i32vec2 &size) {
	gl::glViewport(0, 0, size.x, size.y);
	notify(notification_type::framebuffer_size_changed);
}

void application::on_drop(std::vector<std::string_view> &&paths) {
	spdlog::info("[application::on_drop] Dropped {} path{}",
		paths.size(), paths.size() > 1 ? "s" : "");

	for (size_t i{}; i < paths.size(); ++i) {
		spdlog::info("[application::on_drop]    Path #{:<3}: {}", i, paths[i]);
	}
}

void application::on_error(const int32_t code, const char *message) {
	spdlog::error("[glfw] Error #{:X}: '{}'", code, message);
}

} // namespace gzn::core
