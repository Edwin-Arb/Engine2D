#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include "GameMain.h"

/**
 * Subsystem entry point. Manages OS window generation, input event loops,
 * frame-clock resolution, and execution cascades.
 */
int32_t main()
{
	// Initialize master rendering context window configuration parameters
	sf::RenderWindow Window(sf::VideoMode({ SCREEN_WIDTH, SCREEN_HEIGHT }), "Mini Engine");
	Window.setFramerateLimit(240);

	GameMain GameInstance;
	sf::Clock FrameClock;
	constexpr float FIXED_DT = 1.0f / 240.0f;
	float Accumulator = 0.0f;

	// Load structural asset resources and bind contextual simulation linkages
	GameInstance.InitGame();

	// Core Application Processing Loop Execution Graph
	while (Window.isOpen())
	{
		float DeltaTime = FrameClock.restart().asSeconds();
		DeltaTime = std::min(DeltaTime, 0.125f);
		Accumulator += DeltaTime;

		// Process structural hardware window subsystem events and incoming user actions
		while (const auto Event = Window.pollEvent())
		{
			GameInstance.HandleEvent(*Event);

			if (Event->is<sf::Event::Closed>())
			{
				Window.close();
			}
		}

		while (Accumulator >= FIXED_DT)
		{
			// 1. Tick simulation systems state updates evaluating kinematic frames logic
			GameInstance.Update(Window, FIXED_DT);
			Accumulator -= FIXED_DT;
		}

		// 2. Dispatch frame pipeline execution passes onto the graphics framebuffers
		Window.clear();
		GameInstance.Render(Window);
		Window.display();
	}

	return 0;
}