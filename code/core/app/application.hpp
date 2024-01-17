#pragma once

#include <memory>
#include <glm/vec2.hpp>

#include "core/defaults.hpp"
#include "core/app/game_base.hpp"
#include "core/app/window.hpp"

struct GLFWwindow;

namespace gzn::core {

enum class notification_type;

class application : public window::listener {
public:
	using exit_code_type = int32_t;

	static std::unique_ptr<application> create();

	~application();

	[[nodiscard]] bool initialized() const noexcept;
	[[nodiscard]] bool ready() const noexcept { return initialized() && m_game != nullptr; }

	[[nodiscard]] exit_code_type run();

	void assign_game(const std::shared_ptr<game_base> &game);

	void notify(const notification_type notification) const;

private:
	inline static bool s_instance_exists{ false };
	std::shared_ptr<game_base> m_game{ nullptr };
	std::unique_ptr<window> m_window{ nullptr };

	application();

	void on_close() override;
	void on_focus_changed(const bool focused) override;
	void on_framebuffer_size_changed(const glm::i32vec2 &size) override;
	void on_drop(std::vector<std::string_view> &&paths) override;

	static void on_error(const int32_t code, const char *description);
};

enum class notification_type {
	quit,
	closing,
	focus_lost,
	focus_gained,
	framebuffer_size_changed,
};

} // namespace gzn::core
