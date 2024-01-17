#pragma once

#include <chrono>
#include <glm/vec2.hpp>

#include "core/io/enums.hpp"

namespace gzn::core::io {

using timestamp_type = std::chrono::milliseconds;

timestamp_type make_timestamp() noexcept;

//========================================= COMMON  DATA =========================================//

enum class button_state : uint8_t {
	pressed, released,
	repeating // for keys only
};

struct timestamp_with_state {
	timestamp_type timestamp;
	button_state state;

	timestamp_with_state(const button_state state = button_state::released,
		const timestamp_type _timestamp = make_timestamp()) noexcept;
};

struct position {
	glm::dvec2 value;

	explicit position(const glm::dvec2 _value = { 0.0, 0.0 }) noexcept;
};

//======================================== KEYBOARD  DATA ========================================//

struct key_data : timestamp_with_state {
	key value;

	explicit key_data(const key _value, const button_state _state = button_state::released,
		const timestamp_type _timestamp = make_timestamp()) noexcept;

	explicit key_data(const key _value, const timestamp_with_state state) noexcept;
};

//========================================== MOUSE DATA ==========================================//

struct mouse_button_data : position, timestamp_with_state {
	mouse_button value;

	explicit mouse_button_data(const mouse_button _value,
		const button_state _state = button_state::released,
		const timestamp_type _timestamp = make_timestamp()) noexcept;

	explicit mouse_button_data(const mouse_button _value, const timestamp_with_state state) noexcept;
};

struct mouse_move_data : position {
	explicit mouse_move_data(const glm::dvec2 _value = { 0.0, 0.0 }) noexcept;
	explicit mouse_move_data(const position &pos) noexcept;
};

struct mouse_scroll_data {
	glm::dvec2 delta;
	timestamp_type timestamp;

	mouse_scroll_data(const glm::dvec2 _delta = { 0.0, 0.0 },
		const timestamp_type _timestamp = make_timestamp()) noexcept;
};

//================================================================================================//

} // namespace gzn::core::io
