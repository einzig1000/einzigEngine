#pragma once
#include <vector>
#include <memory>

class UIElement;

class UIScreen
{
public:
	UIScreen() = default;
	virtual ~UIScreen() = default;
	// 画面の更新
	virtual void Update() = 0;
	// 画面の描画
	virtual void Draw() = 0;

protected:
	std::vector<std::unique_ptr<UIElement>> uiElements_;
};

