#include "ParticleManager.h"
#include "DirectX/DirectXManager.h"
#include "Engine/Engine.h"

ParticleManager::ParticleManager(DirectXManager* dxManager_)
{}

ParticleManager::~ParticleManager()
{}

void ParticleManager::Update(const Matrix4x4& viewProjectionMatrix)
{
	// 各パーティクルグループの更新処理
	for (auto& [name, group] : particleGroups_)
	{
		for (auto& particle : group.particles)
		{
			// ビルボード計算
			if (particle.isBillboard)
			{
				// ビルボード処理の実装
			}

			// 寿命に達したパーティクルの削除
			if (particle.liveTime <= 0)
			{
				// パーティクルの削除処理
				continue;
			}

			// 寿命消費
			particle.liveTime -= 1;

			// 速度・加速度による位置更新
			particle.scale.velocity += particle.scale.acceleration * dxManager_->GetDeltaTime();
			particle.scale.value += particle.scale.velocity * dxManager_->GetDeltaTime();
			particle.rotate.velocity += particle.rotate.acceleration * dxManager_->GetDeltaTime();
			particle.rotate.value += particle.rotate.velocity * dxManager_->GetDeltaTime();
			particle.translate.velocity += particle.translate.acceleration * dxManager_->GetDeltaTime();
			particle.translate.value += particle.translate.velocity * dxManager_->GetDeltaTime();

			// ワールド行列・WVP行列の計算
			particle.World = Matrix4x4::MakeAffineMatrix(
				particle.scale.value,
				particle.rotate.value,
				particle.translate.value);
			particle.WVP = particle.World * viewProjectionMatrix; // viewProjectionMatrixは適切に取得する必要があります

			// インスタンシングデータの更新
			group.mappedPtr->scale = particle.scale;
			group.mappedPtr->rotate = particle.rotate;
			group.mappedPtr->translate = particle.translate;
			group.mappedPtr->World = particle.World;
			group.mappedPtr->WVP = particle.WVP;
			group.mappedPtr->liveTime = particle.liveTime;
			group.mappedPtr->color = particle.color;
			group.mappedPtr->isBillboard = particle.isBillboard;
			group.mappedPtr++;
		}
	}
}

void ParticleManager::Draw()
{
	// 各パーティクルグループの描画処理
	for (auto& [name, group] : particleGroups_)
	{
		//Engine::Instance().DrawParticle(group);
	}
}

void ParticleManager::Emit(const std::string& groupName, const ParticleMonoInf& particleInfo)
{}

void ParticleManager::CreateParticleGroup(const std::string& name, uint32_t texture)
{
	// すでに存在する場合は何もしない
	if (particleGroups_.find(name) != particleGroups_.end())
	{
		return;
	}

	ParticleGroup newGroup;

	// テクスチャ設定
	newGroup.texture = texture;

	// インスタンシング用リソースの作成
	newGroup.instanceCount = 1000; // 仮のインスタンス数
	newGroup.instanceResource = CreateBufferResource(
		dxManager_->GetDevice(), sizeof(ParticleMonoInf) * newGroup.instanceCount);

	// インスタンシング用にSRVを確保してインデックスを記録
	newGroup.srvAllocation = dxManager_->GetDescriptorHeapManager()->GetSrvManager()->CreateSRVforStructuredBuffer(
		newGroup.instanceResource.Get(), newGroup.instanceCount, sizeof(ParticleMonoInf));


	//  登録
	particleGroups_[name] = newGroup;
}
