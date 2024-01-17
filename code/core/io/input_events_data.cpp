#include "core/io/input_events_data.hpp"

namespace gzn::core::io {

//========================================= COMMON  DATA =========================================//

timestamp_type make_timestamp() noexcept {
	using namespace std::chrono;
	return duration_cast<timestamp_type>(system_clock::now().time_since_epoch());
}

timestamp_with_state::timestamp_with_state(
		const button_state state, const timestamp_type _timestamp) noexcept
	: timestamp{ _timestamp }, state{ state } {}

position::position(const glm::dvec2 _value) noexcept
	: value{ _value } {}

//======================================== KEYBOARD  DATA ========================================//

key_data::key_data(const key _value, const button_state _state,
		const timestamp_type _timestamp) noexcept
	: timestamp_with_state{ _state, _timestamp }, value{ _value } {}

key_data::key_data(const key _value, const timestamp_with_state state) noexcept
	: timestamp_with_state{ state }, value{ _value } {}

//========================================== MOUSE DATA ==========================================//

mouse_button_data::mouse_button_data(const mouse_button _value,
		const button_state _state, const timestamp_type _timestamp) noexcept
	: timestamp_with_state{ _state, _timestamp }, value{ _value } {}

mouse_button_data::mouse_button_data(const mouse_button _value,
		const timestamp_with_state state) noexcept
	: timestamp_with_state{ state }, value{ _value } {}

mouse_move_data::mouse_move_data(const glm::dvec2 _value) noexcept
	: position{ _value } {}

mouse_move_data::mouse_move_data(const position &pos) noexcept
	: position{ pos.value } {}

mouse_scroll_data::mouse_scroll_data(const glm::dvec2 _delta, const timestamp_type _timestamp) noexcept
	: delta{ _delta }, timestamp{ _timestamp } {}

//================================================================================================//

} // namespace gzn::core::io
