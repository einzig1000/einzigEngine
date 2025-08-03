#pragma once
#include "Map.h"
#include <memory>

class GameManager
{
public:
	GameManager();
	void Update();
	void Draw();


private:



	std::unique_ptr<Map> map_;




};

