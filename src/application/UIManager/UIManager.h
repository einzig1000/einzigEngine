#pragma once
#include "Game.h"

class Player;
class UIScreen;

class UIManager
{
public:
	UIManager(Player* player);
	~UIManager();
	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();

	void CreateScreen(UIMode mode);

	UIMode GetCurrentUIMode() const { return currentUIMode_; }

private:
	UIMode currentUIMode_ = UIMode::None;
	UIScreen* currentScreen_ = nullptr;
	Player* player_ = nullptr;
};

