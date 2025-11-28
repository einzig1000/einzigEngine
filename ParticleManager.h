#pragma once
#include "definition/definition.h"

class DirectXManager;


class ParticleManager
{
public:
	ParticleManager(DirectXManager* dxManager_);
	~ParticleManager();

	void Update(const Matrix4x4& viewProjectionMatrix);
	void Draw();

	void Emit(const std::string& groupName, const ParticleMonoInf& particleInfo);
	void CreateParticleGroup(const std::string& name, uint32_t texture);

	// パーティクルグループ
	std::unordered_map<std::string, ParticleGroup> particleGroups_;

	// DirectXマネージャー
	DirectXManager* dxManager_ = nullptr;
};

