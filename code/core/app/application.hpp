#pragma once

#include <memory>
#include <glm/vec2.hpp>

#include "core/defaults.hpp"
#include "core/app/game_base.hpp"

struct GLFWwindow;

namespace gzn::core {

enum class notification_type;

class application {
public:
	using exit_code_type = int32_t;

	static std::unique_ptr<application> create(
		const glm::ivec2 &size = { defaults::window::width, defaults::window::height },
		const std::string_view title = defaults::window::title);

	~application();

	[[nodiscard]] bool initialized() const noexcept;
	[[nodiscard]] bool ready() const noexcept { return initialized() && game != nullptr; }

	[[nodiscard]] exit_code_type run();

	void assign_game(const std::shared_ptr<game_base> &game);

	void notify(const notification_type notification) const;

private:
	std::shared_ptr<game_base> game{ nullptr };

	application(const glm::ivec2 &size, const std::string_view title);

	static void on_focus_changed(GLFWwindow *window, const int32_t focused);
	static void on_framebuffer_size_changed(GLFWwindow *window, const int32_t width, const int32_t height);
	static void on_drop(GLFWwindow *window, const int32_t count, const char **paths);
	static void on_error(const int32_t code, const char *message);
};

enum class notification_type {
	quit,
	focus_lost,
	focus_gained,
	framebuffer_size_changed,
};

} // namespace gzn::core
