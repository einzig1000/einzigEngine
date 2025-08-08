#include "Title.h"

Title::Title()
{
	nextPhase = PHASE::Phase_None;
}

Title::~Title()
{}


void Title::Initialize()
{
	nextPhase = PHASE::Phase_None;
}

void Title::Update()
{
	if (GetHitKey::keys[DIK_SPACE])
	{
		nextPhase = PHASE::Phase_ActSelect;
	}
}

void Title::Draw()
{

}