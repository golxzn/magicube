#include <core/app/application.hpp>
#include <game/magicube/instance.hpp>

int main() {
	if (auto app{ gzn::core::application::create() }; app != nullptr) {
		app->assign_game(std::make_shared<gzn::game::magicube::instance>());
		return app->run();
	}

	return EXIT_FAILURE;
}
