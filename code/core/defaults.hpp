#pragma once

#include <chrono>
#include <cinttypes>
#include <string_view>

namespace gzn::core::defaults {

namespace window {

	constexpr int32_t width{ 1720 };
	constexpr int32_t height{ 1080 };
	constexpr std::string_view title{ "[magicube]" };

} // namespace window

namespace opengl {

	constexpr int32_t version_major{ 4 };
	constexpr int32_t version_minor{ 4 };

	namespace clear_color {

		constexpr float r{ 0.2f };
		constexpr float g{ 0.2f };
		constexpr float b{ 0.2f };
		constexpr float a{ 1.0f };

	} // namespace clear_color

} // namespace opengl

namespace timeout {

	constexpr std::chrono::milliseconds double_click{ 500 };
	constexpr std::chrono::milliseconds long_press{ 500 };
	constexpr std::chrono::milliseconds just_action{ 10 };

} // namespace timeout

} // namespace gzn::core::defaults
