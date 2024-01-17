#pragma once

#include <chrono>
#include <algorithm>

namespace gzn::core::tools {

template<class ValueType, class ClockType = std::chrono::steady_clock>
class clock {
public:
	using value_type = ValueType;
	using clock_type = ClockType ;
	using time_point = typename clock_type::time_point;

	[[nodiscard]] value_type delta() noexcept {
		const auto now{ clock_type::now() };
		return std::chrono::duration_cast<std::chrono::duration<value_type>>(
			now - std::exchange(m_previous_time, now)).count();
	}

private:
	time_point m_previous_time{ std::chrono::steady_clock::now() };
};

} // namespace gzn::core::tools
