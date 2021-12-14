#pragma once

template<typename InputData, typename OutputData>
class IGame
{
public:
	virtual ~IGame(){}
	virtual OutputData Tick(const InputData& input) = 0;
};
