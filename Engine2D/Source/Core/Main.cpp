#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include "GameMain.h"

/**
 * Application entry point. Creates the window, runs the main loop with a fixed
 * simulation time step, and dispatches input, updates, and rendering.
 */
int32_t main()
{
	// Create the render window and cap the frame rate.
	sf::RenderWindow Window(sf::VideoMode({ SCREEN_WIDTH, SCREEN_HEIGHT }), "Mini Engine");
	Window.setFramerateLimit(240);

	GameMain GameInstance;
	sf::Clock FrameClock;

	// Fixed simulation step: the world is updated in constant FIXED_DT slices,
	// while the accumulator carries leftover real time between frames.
	constexpr float FIXED_DT = 1.0f / 240.0f;
	float Accumulator = 0.0f;

	// Load assets and build the initial scene before the loop starts.
	GameInstance.InitGame();

	// Main loop: input -> fixed-step update(s) -> render, once per frame.
	while (Window.isOpen())
	{
		float DeltaTime = FrameClock.restart().asSeconds();

		// Clamp the frame time so a hitch (e.g. a breakpoint) can't spiral the update count.
		DeltaTime = std::min(DeltaTime, 0.125f);
		Accumulator += DeltaTime;

		// Drain the OS event queue and forward each event to the game.
		while (const auto Event = Window.pollEvent())
		{
			GameInstance.HandleEvent(*Event);

			if (Event->is<sf::Event::Closed>())
			{
				Window.close();
			}
		}

		// Run as many fixed steps as the accumulated time allows (deterministic physics).
		while (Accumulator >= FIXED_DT)
		{
			GameInstance.Update(Window, FIXED_DT);
			Accumulator -= FIXED_DT;
		}

		// Render the current state to the back buffer and present it.
		Window.clear();
		GameInstance.Render(Window);
		Window.display();
	}

	return 0;
}