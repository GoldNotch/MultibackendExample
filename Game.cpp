#include "Game.h"
#include <cmath>
#include <memory>

namespace Game{

	Game::Game(/*here you can pass parameters for game initialization*/)
	{

	}

	Game::~Game()
	{

	}

	OutputData Game::Tick(const InputData& input)
	{
		OutputData output;
		//keys handling
		if (input.PressedKeys[KEY_ESCAPE])
			output.Commands.push_back(COMMAND_CLOSE_GAME);

		//18 values of vertices
		float triangle_vertices[] = {
										-0.5f, -0.5f, -0.5f, sin(input.ProcessTimeInSec), 0.0f, 0.0f,
										0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
										0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f
									};
		output.VertexBuffer.assign(triangle_vertices, triangle_vertices + 18);
		output.VerticesCount = 3;
		return output;
	}

}
