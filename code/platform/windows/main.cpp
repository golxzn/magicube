#include <spdlog/spdlog.h>
#include <core/app/application.hpp>
#include <game/magicube/instance.hpp>

int main() try {
	if (auto app{ gzn::core::application::create() }; app != nullptr) {
		app->assign_game(std::make_shared<gzn::game::magicube::instance>());
		return app->run();
	}

	return EXIT_FAILURE;
} catch (const std::exception &error) {
	spdlog::critical("[main] Uncaught exception: {}", error.what());
	return EXIT_FAILURE;
} catch (...) {
	spdlog::critical("[main] Uncaught unknown exception");
	return EXIT_FAILURE;
}
