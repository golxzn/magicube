#pragma once

#include <array>
#include <bitset>
#include <memory>
#include <magic_enum.hpp>

#include "core/defaults.hpp"
#include "core/io/input_events_data.hpp"

struct GLFWwindow;

namespace gzn::core { class window; }

namespace gzn::core::io {

template<class T, class Data>
using cache = std::enable_if_t<std::is_enum_v<T>, std::array<Data, magic_enum::enum_count<T>()>>;

using modifiers_bitset = std::bitset<magic_enum::enum_count<modifier>()>;

class inputs {
public:
	inputs() = delete;

	static void assign(const window &win);
	static void reset(const window &win);
	static void clear() noexcept;

	[[nodiscard]] static bool pressed(const key key_value) noexcept;
	[[nodiscard]] static bool released(const key key_value) noexcept;

	[[nodiscard]] static bool just_pressed(const key key_value,
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;
	[[nodiscard]] static bool just_released(const key key_value,
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	[[nodiscard]] static bool pressed(const mouse_button button) noexcept;
	[[nodiscard]] static bool released(const mouse_button button) noexcept;

	[[nodiscard]] static bool just_pressed(const mouse_button button,
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;
	[[nodiscard]] static bool just_released(const mouse_button button,
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	[[nodiscard]] static bool just_scrolled(
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;
	[[nodiscard]] static bool just_vertical_scrolled(
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;
	[[nodiscard]] static bool just_horizontal_scrolled(
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	[[nodiscard]] static bool has_modifier(const modifier mod) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool has_modifiers() noexcept;

	[[nodiscard]] static glm::ivec2 mouse_position() noexcept;
	[[nodiscard]] static glm::dvec2 cursor_position() noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool pressed(const key key_value) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool released(const key key_value) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool just_pressed(const key key_value,
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool just_released(const key key_value,
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool pressed(const mouse_button button) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool released(const mouse_button button) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool just_pressed(const mouse_button button,
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool just_released(const mouse_button button,
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool just_scrolled(
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool just_vertical_scrolled(
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

	template<modifier ...Modifiers>
	[[nodiscard]] static bool just_horizontal_scrolled(
		const std::chrono::milliseconds interval = defaults::timeout::just_action) noexcept;

private:
	inline static modifiers_bitset modifiers{};
	inline static cache<key,          timestamp_with_state> keys{};
	inline static cache<mouse_button, timestamp_with_state> mouse_buttons{};
	inline static glm::dvec2 current_cursor_position{ 0.0 };
	inline static mouse_scroll_data last_scroll{};

	static void update_modifiers(const int32_t value) noexcept;

	static void key_callback(GLFWwindow *window, int key, int scan_code, int action, int mods) noexcept;
	static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) noexcept;
	static void cursor_position_callback(GLFWwindow *window, double x, double y) noexcept;
	static void scroll_callback(GLFWwindow *window, double x, double y) noexcept;
};

template<modifier ...Modifiers>
bool inputs::has_modifiers() noexcept {
	constexpr size_t modifiers_count{ sizeof...(Modifiers) };
	if constexpr (modifiers_count > 0) {
		static constexpr modifiers_bitset input_modifiers{
			(... | magic_enum::enum_underlying(Modifiers))
		};
		return modifiers == (modifiers & input_modifiers);
	} else {
		return false;
	}
}

template<modifier ...Modifiers>
bool inputs::pressed(const key key_value) noexcept {
	return has_modifiers<Modifiers...>() && pressed(key_value);
}

template<modifier ...Modifiers>
bool inputs::released(const key key_value) noexcept {
	return has_modifiers<Modifiers...>() && released(key_value);
}

template<modifier ...Modifiers>
bool inputs::just_pressed(const key key_value, const std::chrono::milliseconds interval) noexcept {
	return has_modifiers<Modifiers...>() && just_pressed(key_value, interval);
}

template<modifier ...Modifiers>
bool inputs::just_released(const key key_value, const std::chrono::milliseconds interval) noexcept {
	return has_modifiers<Modifiers...>() && just_released(key_value, interval);
}

template<modifier ...Modifiers>
bool inputs::pressed(const mouse_button button) noexcept {
	return has_modifiers<Modifiers...>() && pressed(button);
}

template<modifier ...Modifiers>
bool inputs::released(const mouse_button button) noexcept {
	return has_modifiers<Modifiers...>() && released(button);
}

template<modifier ...Modifiers>
bool inputs::just_pressed(const mouse_button button, const std::chrono::milliseconds interval) noexcept {
	return has_modifiers<Modifiers...>() && just_pressed(button, interval);
}

template<modifier ...Modifiers>
bool inputs::just_released(const mouse_button button, const std::chrono::milliseconds interval) noexcept {
	return has_modifiers<Modifiers...>() && just_released(button, interval);
}

template<modifier ...Modifiers>
bool inputs::just_scrolled(const std::chrono::milliseconds interval) noexcept {
	return has_modifiers<Modifiers...>() && just_scrolled(interval);
}

template<modifier ...Modifiers>
bool inputs::just_vertical_scrolled(const std::chrono::milliseconds interval) noexcept {
	return has_modifiers<Modifiers...>() && just_vertical_scrolled(interval);
}

template<modifier ...Modifiers>
bool inputs::just_horizontal_scrolled(const std::chrono::milliseconds interval) noexcept {
	return has_modifiers<Modifiers...>() && just_horizontal_scrolled(interval);
}

} // namespace gzn::core::io
