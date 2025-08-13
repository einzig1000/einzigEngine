#include "ActSelect.h"

ActSelect::ActSelect()
{
	nextPhase = PHASE::Phase_None;

	iconSum_ = 3;

	center.model = uint32_t(TEXTURE::Phase_Buttle);
	center.texture = uint32_t(TEXTURE::Phase_Buttle);
	center.color = 0x00000000;


	Vector3 IconSum[3];
	const float angleIncrement = 2.0f * std::numbers::pi / iconSum_;
	for (int i = 0; i < iconSum_; ++i)
	{
		float angle = angleIncrement * i;
		IconSum[i].x = -std::sin(angle);
		IconSum[i].y = 0.0f;
		IconSum[i].z = -std::cos(angle);
		IconSum[i] *= 30;
	}
	

	ButtleIcon.model = uint32_t(TEXTURE::Phase_Buttle);
	ButtleIcon.texture = uint32_t(TEXTURE::Phase_Buttle);
	ButtleIcon.transforms.rotate.y = (((2.0f * std::numbers::pi) / iconSum_) * 0) + (std::numbers::pi);
	ButtleIcon.transforms.translate += IconSum[0];
	ButtleIcon.transforms.parentWorld = &center.transforms.World;

	GatyaIcon.model = uint32_t(TEXTURE::Phase_Gatya);
	GatyaIcon.texture = uint32_t(TEXTURE::Phase_Gatya);
	GatyaIcon.color = 0x00FF00FF;
	GatyaIcon.transforms.rotate.y = (((2.0f * std::numbers::pi) / iconSum_) * 1) + (std::numbers::pi);
	GatyaIcon.transforms.translate += IconSum[1];
	GatyaIcon.transforms.parentWorld = &center.transforms.World;

	UnitOverViewIcon.model = uint32_t(TEXTURE::Phase_UnitOverview);
	UnitOverViewIcon.texture = uint32_t(TEXTURE::Phase_UnitOverview);
	UnitOverViewIcon.color = 0x0000FFFF;
	UnitOverViewIcon.transforms.rotate.y = (((2.0f * std::numbers::pi) / iconSum_) * 2) + (std::numbers::pi);
	UnitOverViewIcon.transforms.translate += IconSum[2];
	UnitOverViewIcon.transforms.parentWorld = &center.transforms.World;
}

ActSelect::~ActSelect()
{}

void ActSelect::Initialize()
{
	nextPhase = PHASE::Phase_None;

	Game::MoveCenterTarget({ 0.0f, 0.0f, 0.0f }, 0);
	Game::MoveDistanceTarget(50.0f, 0);
	Game::MoveRotateTarget({ 0.0f, 0.0f, 0.0f }, 0);
}

void ActSelect::Update()
{
	if ((GetHitKey::keys[DIK_RIGHTARROW] || GetHitKey::keys[DIK_D]) && easingFrame > easingFrameMAX)
	{
		if (easingFrame < easingFrameMAX + 2 && easingFrameMAX > 10)easingFrameMAX -= 10;
		preCenterRotate = center.transforms.rotate;
		targetCenterRotate = preCenterRotate;
		targetCenterRotate.y += (2.0f * std::numbers::pi) / iconSum_;
		easingFrame = 0;
		selectIcon++;
		if (selectIcon >= iconSum_)selectIcon = 0;
	}
	else if ((GetHitKey::keys[DIK_LEFTARROW] || GetHitKey::keys[DIK_A]) && easingFrame > easingFrameMAX)
	{
		if (easingFrame < easingFrameMAX + 2 && easingFrameMAX > 10)easingFrameMAX -= 10;
		preCenterRotate = center.transforms.rotate;
		targetCenterRotate = preCenterRotate;
		targetCenterRotate.y -= (2.0f * std::numbers::pi) / iconSum_;
		easingFrame = 0;
		selectIcon--;
		if (selectIcon < 0)selectIcon = iconSum_ - 1;
	}
	if (easingFrame > easingFrameMAX && easingFrameMAX < 60)
	{
		easingFrame = 60;
		easingFrameMAX = 60;
	}

	if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE] && easingFrame > easingFrameMAX - 5)
	{
		// 演出ついksするならここ
		if (selectIcon % iconSum_ == 0)nextPhase = PHASE::Phase_StageSelect;
		else if (selectIcon % iconSum_ == 1)nextPhase = PHASE::Phase_Gatya;
		else if (selectIcon % iconSum_ == 2)nextPhase = PHASE::Phase_UnitOverview;
	}


	easingFrame++;
	center.transforms.rotate.y = Easings::F_OUT_SINE(preCenterRotate.y, targetCenterRotate.y, easingFrame / easingFrameMAX);

	ImGui::Begin("act");
	ImGui::Text("%d", selectIcon);
	ImGui::Text("%d", selectIcon % iconSum_);
	ImGui::End();
}

void ActSelect::Draw()
{
	center.DrawAABB();
	ButtleIcon.Draw();
	GatyaIcon.Draw();
	UnitOverViewIcon.Draw();
}