#pragma once
#include "Game.h"

class Player;
class UIScreen;
class PlayingScreen;
class InventoryScreen;

class UIManager
{
public:
	UIManager(Player* player);
	~UIManager();
	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();

	void ChangeScreen(UIMode mode);

	UIMode GetCurrentUIMode() const { return currentUIMode_; }

private:
	UIMode currentUIMode_ = UIMode::None;

	UIScreen* currentScreen_ = nullptr;

	PlayingScreen* playingScreen_ = nullptr;
	InventoryScreen* inventoryScreen_ = nullptr;

	Player* player_ = nullptr;
};

