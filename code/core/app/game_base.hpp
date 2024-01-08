#pragma once

namespace gzn::core {

enum class notification_type;

class game_base {
public:
	virtual ~game_base() = default;

	virtual void start() = 0;
	virtual void update(const double delta) = 0;
	virtual void draw() = 0;
	virtual void stop() = 0;

	virtual void pause() = 0;
	virtual void resume() = 0;

	virtual void notification(const notification_type type) = 0;
};

} // namespace gzn::core
