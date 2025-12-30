#pragma once
#include <vector>
#include <memory>
#include "Game.h"

class UIElement;

class UIScreen
{
public:
	UIScreen() = default;
	virtual ~UIScreen() = default;
	// 画面の初期化
	virtual void Initialize() = 0;
	// 画面の更新
	virtual void Update() = 0;
	// 画面の描画
	virtual void Draw() = 0;

	virtual UIMode GetNextUIMode() const { return nextUIMode_; }

protected:
	UIMode nextUIMode_;
	std::vector<std::unique_ptr<UIElement>> uiElements_;
};

