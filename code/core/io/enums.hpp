#pragma once

#include <cinttypes>

namespace gzn::core::io {

enum class key : uint8_t {
// Printable keys
	unknown,
	space,
	apostrophe,
	comma,
	minus,
	period,
	slash,
	n0, n1, n2, n3, n4, n5, n6, n7, n8, n9,
	semicolon,
	equal,
	a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
	left_bracket,
	right_bracket,
	backslash,
	grave_accent,

// Function keys
	escape,
	enter,
	tab,
	backspace,
	insert,
	del,
	right, left, down, up,
	page_up, page_down,
	home, end,
	caps_lock,
	scroll_lock,
	num_lock,
	print_screen,
	pause,

	f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13,
	f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25,

// Keypad
	keypad_0, keypad_1, keypad_2, keypad_3, keypad_4,
	keypad_5, keypad_6, keypad_7, keypad_8, keypad_9,

	keypad_decimal,
	keypad_divide,
	keypad_multiply,
	keypad_subtract,
	keypad_add,
	keypad_enter,
	keypad_equal,

// Modifier keys
	left_shift,
	left_control,
	left_alt,
	left_super,
	right_shift,
	right_control,
	right_alt,
	right_super,

	menu,
};

enum class mouse_button : uint8_t {
	b1 = 0u,
	b2 = 1u,
	b3 = 2u,
	b4 = 3u,
	b5 = 4u,
	b6 = 5u,
	b7 = 6u,
	b8 = 7u,

	left = b1,
	right = b2,
	middle = b3,
};

enum class modifier : uint8_t {
	shift     = 0x01,
	control   = 0x02,
	alt       = 0x04,
	super     = 0x08,
	caps_lock = 0x10,
	num_lock  = 0x20,
};
using mod = modifier;

} // namespace gzn::core::io
