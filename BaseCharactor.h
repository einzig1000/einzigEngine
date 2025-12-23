#pragma once
#include "Game.h"

class MapManager;

class BaseCharactor
{
public:
	virtual ~BaseCharactor() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawImGui() = 0;
	virtual void SetMapManager(MapManager* mapManager);
	virtual void ResolveMapCollision();
	virtual void Jump();


protected:
	MapManager* mapManager_ = nullptr;

	RenderData_Model data_;

	float jumpPower_ = 0.1491f;
};

