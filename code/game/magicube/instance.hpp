#pragma once

#include <string_view>
#include <glm/mat4x4.hpp>
#include <core/app/game_base.hpp>

namespace gzn::game::magicube {

class instance final : public core::game_base {
public:
	~instance() = default;

	void start() override;
	void update(const double delta) override;
	void draw() override;
	void stop() override;

	void pause() override;
	void resume() override;

	void notification(const core::notification_type type) override;

private:
	bool paused{ false };

	uint32_t VAO{};
	uint32_t VBO{};
	uint32_t EBO{};
	uint32_t program{};

	glm::mat4x4 model{ 1.0f };
	glm::mat4x4 view{ 1.0f };
	glm::mat4x4 projection{ 1.0f };

	void set_matrix(const std::string_view name, const glm::mat4x4 &value);
	static glm::mat4x4 make_projection();
};

} // namespace gzn::game::magicube
