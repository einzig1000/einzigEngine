#pragma once
#include <sstream>

class Map
{
public:
	Map();
	void Initialize(int stageNum);

	void Update();
	void Draw();


private:
	std::stringstream mapCSV[10];
};