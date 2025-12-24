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

	virtual RenderData_Model& GetRenderData() { return data_; }


	RenderData_Model data_;
protected:
	MapManager* mapManager_ = nullptr;


	float jumpPower_ = 0.1491f;
};

