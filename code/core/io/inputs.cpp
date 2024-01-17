#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "core/io/inputs.hpp"
#include "core/app/window.hpp"
#include "core/app/application.hpp"

namespace gzn::core::io {

namespace {

constexpr button_state to_state(const int32_t value) noexcept;
constexpr key to_key(const int32_t value) noexcept;
constexpr mouse_button to_mouse_button(const int32_t value) noexcept;

} // anonymous namespace

void inputs::assign(const window &win) {
	if (!win.valid()) {
		spdlog::error("[inputs::assign] Window is invalid");
		return;
	}

	glfwSetKeyCallback(win.handle(),         &inputs::key_callback);
	glfwSetMouseButtonCallback(win.handle(), &inputs::mouse_button_callback);
	glfwSetCursorPosCallback(win.handle(),   &inputs::cursor_position_callback);
	glfwSetScrollCallback(win.handle(),      &inputs::scroll_callback);
}

void inputs::reset(const window &win) {
	if (!win.valid()) {
		spdlog::warn("[inputs::reset] Reset called, but there's no window context");
		return;
	}

	glfwSetKeyCallback(win.handle(), nullptr);
	glfwSetMouseButtonCallback(win.handle(), nullptr);
	glfwSetCursorPosCallback(win.handle(), nullptr);
	glfwSetScrollCallback(win.handle(), nullptr);

	clear();
}

void inputs::clear() noexcept {
	keys.fill(timestamp_with_state{});
	mouse_buttons.fill(timestamp_with_state{});

	current_cursor_position = glm::dvec2{ 0.0 };
	last_scroll = mouse_scroll_data{ glm::dvec2{ 0.0 } };
}

bool inputs::pressed(const key key_value) noexcept {
	if (const auto index{ static_cast<size_t>(key_value) }; index < keys.size()) {
		return keys[index].state == button_state::pressed;
	}
	return false;
}

bool inputs::released(const key key_value) noexcept {
	if (const auto index{ static_cast<size_t>(key_value) }; index < keys.size()) {
		return keys[index].state == button_state::released;
	}
	return false;
}

bool inputs::just_pressed(const key key_value, const std::chrono::milliseconds interval) noexcept {
	if (const auto index{ static_cast<size_t>(key_value) }; index < keys.size()) {
		const auto &data{ keys[index] };

		return (data.state == button_state::pressed)
			&& ((data.timestamp + interval) >= make_timestamp());
	}
	return false;
}

bool inputs::just_released(const key key_value, const std::chrono::milliseconds interval) noexcept {
	if (const auto index{ static_cast<size_t>(key_value) }; index < keys.size()) {
		const auto &data{ keys[index] };

		return (data.state == button_state::released)
			&& ((data.timestamp + interval) >= make_timestamp());
	}
	return false;
}

bool inputs::pressed(const mouse_button button) noexcept {
	if (const auto index{ static_cast<size_t>(button) }; index < mouse_buttons.size()) {
		return mouse_buttons[index].state == button_state::pressed;
	}
	return false;
}

bool inputs::released(const mouse_button button) noexcept {
	if (const auto index{ static_cast<size_t>(button) }; index < mouse_buttons.size()) {
		return mouse_buttons[index].state == button_state::released;
	}
	return false;
}

bool inputs::just_pressed(const mouse_button button, const std::chrono::milliseconds interval) noexcept {
	if (const auto index{ static_cast<size_t>(button) }; index < mouse_buttons.size()) {
		const auto &data{ mouse_buttons[index] };

		return (data.state == button_state::pressed)
			&& ((data.timestamp + interval) >= make_timestamp());
	}
	return false;
}

bool inputs::just_released(const mouse_button button, const std::chrono::milliseconds interval) noexcept {
	if (const auto index{ static_cast<size_t>(button) }; index < mouse_buttons.size()) {
		const auto &data{ mouse_buttons[index] };

		return (data.state == button_state::released)
			&& ((data.timestamp + interval) >= make_timestamp());
	}
	return false;
}

bool inputs::just_scrolled(const std::chrono::milliseconds interval) noexcept {
	return (last_scroll.timestamp + interval) >= make_timestamp();
}

bool inputs::just_vertical_scrolled(const std::chrono::milliseconds interval) noexcept {
	return last_scroll.delta.y != 0.0 && just_scrolled(interval);
}

bool inputs::just_horizontal_scrolled(const std::chrono::milliseconds interval) noexcept {
	return last_scroll.delta.x != 0.0 && just_scrolled(interval);
}

bool inputs::has_modifier(const modifier mod) noexcept {
	return modifiers.test(magic_enum::enum_index(mod).value_or(0));
}

glm::ivec2 inputs::mouse_position() noexcept {
	return glm::ivec2{ current_cursor_position };
}

glm::dvec2 inputs::cursor_position() noexcept {
	return current_cursor_position;
}


void inputs::update_modifiers(const int32_t integer_value) noexcept {
	modifiers = modifiers_bitset{ static_cast<std::underlying_type_t<modifier>>(0xFF & integer_value) };
}

void inputs::key_callback(GLFWwindow *window, const int key, const int scan_code, const int action, const int mods) noexcept {
	const auto key_value{ to_key(key) };
	auto &data{ keys.at(static_cast<size_t>(key_value)) };
	data = timestamp_with_state{ to_state(action) };

	update_modifiers(mods);
}

void inputs::mouse_button_callback(GLFWwindow *window, const int button, const int action, const int mods) noexcept {
	const auto button_value{ to_mouse_button(button) };
	auto &data{ mouse_buttons.at(static_cast<size_t>(button_value)) };
	data = timestamp_with_state{ to_state(action) };

	update_modifiers(mods);
}

void inputs::cursor_position_callback(GLFWwindow *window, const double x, const double y) noexcept {
	current_cursor_position = glm::dvec2{ x, y };
}

void inputs::scroll_callback(GLFWwindow *window, const double x, const double y) noexcept {
	last_scroll = mouse_scroll_data{ glm::dvec2{ x, y } };
}


namespace {

constexpr bool _in_range(const int32_t value, const int32_t lower, const int32_t upper) noexcept {
	return value >= lower && value <= upper;
}

template<class T>
constexpr T _migrate(const int32_t value, const int32_t lower_value, const T lower_target) noexcept {
	return static_cast<T>(value - lower_value + static_cast<int32_t>(lower_target));
}

constexpr button_state to_state(const int32_t value) noexcept {
	switch (value) {
		case GLFW_PRESS:   return button_state::pressed;
		case GLFW_REPEAT:  return button_state::repeating;
		case GLFW_RELEASE: return button_state::released;
		default:           return button_state::released;
	}
	return button_state::released;
}

constexpr key to_key(const int32_t value) noexcept {
	if      (_in_range(value, GLFW_KEY_0, GLFW_KEY_9))                    return _migrate(value, GLFW_KEY_0, key::n0);
	else if (_in_range(value, GLFW_KEY_A, GLFW_KEY_Z))                    return _migrate(value, GLFW_KEY_A, key::a);
	else if (_in_range(value, GLFW_KEY_F1, GLFW_KEY_F25))                 return _migrate(value, GLFW_KEY_F1, key::f1);
	else if (_in_range(value, GLFW_KEY_KP_0, GLFW_KEY_KP_EQUAL))          return _migrate(value, GLFW_KEY_KP_0, key::keypad_0);
	else if (_in_range(value, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SUPER)) return _migrate(value, GLFW_KEY_LEFT_SHIFT, key::left_shift);

	switch (value) {
		case GLFW_KEY_SPACE:          return key::space;
		case GLFW_KEY_APOSTROPHE:     return key::apostrophe;
		case GLFW_KEY_COMMA:          return key::comma;
		case GLFW_KEY_MINUS:          return key::minus;
		case GLFW_KEY_PERIOD:         return key::period;
		case GLFW_KEY_SLASH:          return key::slash;
		case GLFW_KEY_SEMICOLON:      return key::semicolon;
		case GLFW_KEY_EQUAL:          return key::equal;
		case GLFW_KEY_LEFT_BRACKET:   return key::left_bracket;
		case GLFW_KEY_RIGHT_BRACKET:  return key::right_bracket;
		case GLFW_KEY_BACKSLASH:      return key::backslash;
		case GLFW_KEY_GRAVE_ACCENT:   return key::grave_accent;
		case GLFW_KEY_ESCAPE:         return key::escape;
		case GLFW_KEY_ENTER:          return key::enter;
		case GLFW_KEY_TAB:            return key::tab;
		case GLFW_KEY_BACKSPACE:      return key::backspace;
		case GLFW_KEY_INSERT:         return key::insert;
		case GLFW_KEY_DELETE:         return key::del;
		case GLFW_KEY_RIGHT:          return key::right;
		case GLFW_KEY_LEFT:           return key::left;
		case GLFW_KEY_DOWN:           return key::down;
		case GLFW_KEY_UP:             return key::up;
		default: break;
	}

	switch (value) {
		case GLFW_KEY_PAGE_UP:        return key::page_up;
		case GLFW_KEY_PAGE_DOWN:      return key::page_down;
		case GLFW_KEY_HOME:           return key::home;
		case GLFW_KEY_END:            return key::end;
		case GLFW_KEY_CAPS_LOCK:      return key::caps_lock;
		case GLFW_KEY_SCROLL_LOCK:    return key::scroll_lock;
		case GLFW_KEY_NUM_LOCK:       return key::num_lock;
		case GLFW_KEY_PRINT_SCREEN:   return key::print_screen;
		case GLFW_KEY_PAUSE:          return key::pause;
		case GLFW_KEY_MENU:           return key::menu;
		default: break;
	}
	return key::unknown;
}

constexpr mouse_button to_mouse_button(const int32_t value) noexcept {
	if (_in_range(value, GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_LAST)) {
		return _migrate(value, GLFW_MOUSE_BUTTON_LEFT, mouse_button::left);
	}
	return mouse_button::b8;
}

} // anonymous namespace


} // namespace gzn::core::io
