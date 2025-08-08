#include "Gatya.h"
#include "CharacterManager.h"
#include "Charactor.h"

Gatya::Gatya(CharacterManager* characterManager)
    : characterManager_(characterManager)
{}

Gatya::~Gatya()
{}

void Gatya::Initialize()
{
    nextPhase = PHASE::Phase_None;


}

void Gatya::Update()
{
}

void Gatya::Draw()
{
}

void Gatya::AddCharactor(Charactor* newChar)
{
    characterManager_->AddCharactorList(newChar);
}
