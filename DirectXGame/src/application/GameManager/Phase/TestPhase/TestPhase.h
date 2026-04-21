#pragma once
#include "GameManager/Phase/PhaseParent/PhaseParent.h"
#include <memory>

class modelDrawer : public RenderObject
{
public:
	modelDrawer();
	~modelDrawer();
	void Update() override;
	void Draw() override;

};


class TestPhase :
	public PhaseParent
{
public:
	TestPhase();
	~TestPhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }



private:
	std::unique_ptr<RenderObject> renderObject1_;

	//std::unique_ptr<RenderData_Model> ground_;
	//std::unique_ptr<RenderData_Model> wall1_;
	//std::unique_ptr<RenderData_Model> wall2_;
	//std::unique_ptr<RenderData_Model> wall3_;
	//std::unique_ptr<RenderData_Model> wall4_;
	//std::unique_ptr<RenderData_Model> player_;

	//std::unique_ptr<RenderData_Model> shoulder_;
	//std::unique_ptr<RenderData_Model> elbow_;
	//std::unique_ptr<RenderData_Model> hand_;

	//std::unique_ptr<RenderData_Rect> rect_;

	//std::unique_ptr<RenderData_Sprite> sprite1_;
	//std::unique_ptr<RenderData_Sprite> sprite2_;

	//std::unique_ptr<RenderData_Particle> particle1_;

	//std::unique_ptr<RenderData_Triangle> triangle1_;
	//std::unique_ptr<RenderData_Triangle> triangle2_;

	//std::unique_ptr<RenderData_Line> line_;
	//std::unique_ptr<RenderData_Line> line2_;
	//std::unique_ptr<RenderData_Line> line3_;

	Sphere sphere1_;
	SphereXYZ sphereXYZ1_;
	Cylinder cylinder1_;
	AABB aabb1_;

	Coordinate_cylindrical cylindricalPos_;
	Vector3 targetPos_;

	float roll_ = 0.0f;

	uint32_t audio1;
	uint32_t audio2;


};