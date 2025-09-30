#include "StageSelect.h"

StageSelect::StageSelect()
{
    nextPhase = PHASE::Phase_None;

	iconSum_ = STAGE_MAX;

	center.model = uint32_t(TEXTURE::StageSelect_icon);
	center.texture = uint32_t(TEXTURE::StageSelect_icon);
	center.color = 0x00000000;

	Vector3 IconSum[STAGE_MAX];

	const float angleIncrement = 2.0f * float(std::numbers::pi) / iconSum_;
	for (int i = 0; i < iconSum_; ++i)
	{
		float angle = angleIncrement * i;
		IconSum[i].x = -std::sin(angle);
		IconSum[i].y = 0.0f;
		IconSum[i].z = -std::cos(angle);
		IconSum[i] *= 30;
		Icon_[i].model = uint32_t(TEXTURE::StageSelect_icon);
		Icon_[i].texture = uint32_t(TEXTURE::StageSelect_icon);
		Icon_[i].transforms.rotate.y = (((2.0f * float(std::numbers::pi)) / iconSum_) * i) + (float(std::numbers::pi));
		Icon_[i].transforms.translate += IconSum[i];
		Icon_[i].transforms.parentWorld = &center.transforms.World;
	}
}

StageSelect::~StageSelect()
{}

void StageSelect::Initialize()
{
    nextPhase = PHASE::Phase_None;

	Game::MoveCenterTarget({ 0.0f, 0.0f, 0.0f }, 0 , EaseType::OUT_QUART);
	Game::MoveDistanceTarget(60.0f, 0, EaseType::OUT_QUART);
	Game::MoveRotateTarget({ 0.0f, 0.0f, 0.0f }, 0, EaseType::OUT_QUART);
}

void StageSelect::Update()
{
	if ((GetHitKey::keys[DIK_RIGHTARROW] || GetHitKey::keys[DIK_D]) && easingFrame > easingFrameMAX)
	{
		if (easingFrame < easingFrameMAX + 2 && easingFrameMAX > 10)easingFrameMAX -= 10;
		preCenterRotate = center.transforms.rotate;
		targetCenterRotate = preCenterRotate;
		targetCenterRotate.y += (2.0f * float(std::numbers::pi)) / iconSum_;
		easingFrame = 0;
		selectIcon++;
		if (selectIcon >= iconSum_)selectIcon = 0;
	}
	else if ((GetHitKey::keys[DIK_LEFTARROW] || GetHitKey::keys[DIK_A]) && easingFrame > easingFrameMAX)
	{
		if (easingFrame < easingFrameMAX + 2 && easingFrameMAX > 10)easingFrameMAX -= 10;
		preCenterRotate = center.transforms.rotate;
		targetCenterRotate = preCenterRotate;
		targetCenterRotate.y -= (2.0f *float(std::numbers::pi)) / iconSum_;
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
		nextPhase = PHASE::Phase_GameScene;

		//if (selectIcon == 0)nextPhase = PHASE::Phase_StageSelect;
		//else if (selectIcon == 1)nextPhase = PHASE::Phase_Gatya;
		//else if (selectIcon == 2)nextPhase = PHASE::Phase_UnitOverview;
	}


	easingFrame++;
	center.transforms.rotate.y = Easings::EasingFloat(preCenterRotate.y, targetCenterRotate.y, EaseType::OUT_SINE, easingFrame / easingFrameMAX);

	ImGui::Begin("act");
	ImGui::Text("%d", selectIcon);
	ImGui::Text("%d", selectIcon % iconSum_);
	ImGui::Text("%f", center.transforms.rotate.y);
	ImGui::End();

}

void StageSelect::Draw()
{
	center.Draw();
	for (int i = 0; i < iconSum_; ++i)
	{
		Icon_[i].Draw();
	}
}