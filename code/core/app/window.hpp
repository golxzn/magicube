#pragma once

#include <string>
#include <bitset>
#include <vector>

#include <glm/vec2.hpp>
#include <magic_enum.hpp>

#include "core/defaults.hpp"

struct GLFWwindow;

namespace gzn::core {

class window {
public:
	class listener;

	window(const glm::i32vec2 &size = { defaults::window::width, defaults::window::height },
		const std::string_view title = defaults::window::title,
		const bool full_screen = defaults::window::full_screen);

	~window();

	window(const window &other) = delete;
	window(window &&other) noexcept;
	window &operator=(const window &other) = delete;
	window &operator=(window &&other) noexcept;

	void set_listener(listener &_listener) noexcept;
	void reset_listener() noexcept;

	void close() const noexcept;
	void poll_events() const noexcept;
	void swap_buffers() const noexcept;

	void set_title(const std::string_view title) noexcept;

	[[nodiscard]] bool valid() const noexcept;
	[[nodiscard]] glm::i32vec2 size() const noexcept;
	[[nodiscard]] std::string_view title() const noexcept;

	[[nodiscard]] bool should_close() const noexcept;

	[[nodiscard]] GLFWwindow *handle() const noexcept { return m_handle; }

private:
	GLFWwindow *m_handle{ nullptr };
	listener *m_listener{ nullptr };
	std::string m_title;

	static window *load_myself(GLFWwindow *window) noexcept;

	static void on_close(GLFWwindow *window);
	static void on_focus_changed(GLFWwindow *window, const int32_t focused);
	static void on_framebuffer_size_changed(GLFWwindow *window, const int32_t width, const int32_t height);
	static void on_drop(GLFWwindow *window, const int32_t count, const char **paths);
};


class window::listener {
public:
	virtual ~listener() = default;

	virtual void on_close() = 0;
	virtual void on_focus_changed(const bool focused) = 0;
	virtual void on_framebuffer_size_changed(const glm::i32vec2 &size) = 0;

	enum class additional_event : uint8_t {
		destroying,
		drop,
	};
	using events_bitset = std::bitset<magic_enum::enum_count<additional_event>()>;

	virtual void on_destroying() {};
	virtual void on_drop(std::vector<std::string_view> &&files) {}



	template<additional_event ...other>
	void listen_to_additional_event() noexcept {
		static_assert(sizeof...(other) > 0, "At least one event should be specified");

		m_additional_events = events_bitset{ (... | magic_enum::enum_underlying(other)) };
	}

	template<additional_event ...other>
	[[nodiscard]] bool has_additional_event() const noexcept {
		static_assert(sizeof...(other) > 0, "At least one event should be specified");
		return (m_additional_events.test(magic_enum::enum_integer(other)) || ...);
	}

private:
	events_bitset m_additional_events;
};

} // namespace gzn::core
