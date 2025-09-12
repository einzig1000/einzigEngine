#include "GameManager.h"

GameManager::GameManager()
{
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // ドッキング機能を有効化
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // ウィンドウを複数の OS ウィンドウ化
}

GameManager::~GameManager()
{}

void GameManager::Update()
{

}

void GameManager::Draw()
{
	test.Draw();
	test.DrawImGui();
}