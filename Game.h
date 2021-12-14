#pragma once
#include <vector>
#include "IGame.h"

namespace Game
{

	enum KeyCode
	{
		KEY_UNKNOWN,
		KEY_ESCAPE,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		//...
		KEY_W,
		KEY_A,
		KEY_S,
		KEY_D,
		KEY_SPACE,
		KEY_LEFT_MOUSE_BUTTON,
		KEY_RIGHT_MOUSE_BUTTON,
		//...
		KEYS_TOTAL
	};

	enum ErrorCode
	{
		ERROR_UNKNOWN,
		//..
		ERRORS_TOTAL
	};

	enum CommandCode
	{
		COMMAND_UNKNOWN,
		COMMAND_CLOSE_GAME,
		COMMAND_SAVE_GAME,
		COMMAND_LOAD_GAME,
		//any other commands you want
		COMMANDS_TOTAL
	};

	struct InputData
	{
		float ProcessTimeInSec;
		float CursorPos_x;
		float CursorPos_y;
		bool PressedKeys[KeyCode::KEYS_TOTAL];
	};

	struct OutputData
	{
		std::vector<CommandCode> Commands;
		std::vector<ErrorCode> Errors;
		//RenderingData
		std::vector<float> VertexBuffer;
		int VerticesCount;
	};


	class Game : public IGame<InputData, OutputData>
	{
	public:
		//game constructor for initialization
		Game(/*here you can pass parameters for game initialization*/);
		//game destructor for free resources
		virtual ~Game();
		//game loop iteration
		virtual OutputData Tick(const InputData& input) override;
	};
}
