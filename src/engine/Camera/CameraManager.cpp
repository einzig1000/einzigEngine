#include "CameraManager.h"
#include "Camera.h"
#include "Facade/Game.h"

CameraManager::CameraManager()
{
	Camera ReleaseCamera;
	ReleaseCamera.name_ = "ReleaseCamera";
	ReleaseCamera.enableControl_ = false;
	camera_.push_back(ReleaseCamera);

	Camera DebugCamera;
	DebugCamera.name_ = "DebugCamera";
	DebugCamera.enableControl_ = true;
	camera_.push_back(DebugCamera);


}

CameraManager::~CameraManager()
{
}

void CameraManager::AddCamera(const std::string name, bool enableControl)
{
	Camera def;
	def.name_ = name;
	def.enableControl_ = enableControl;
	camera_.push_back(def);
}

void CameraManager::Update()
{
	camera_[currentCameraID_].Update();
}

void CameraManager::Draw()
{
	ImGui::Begin("Camera");
	camera_[currentCameraID_].DrawImGui();
	ImGui::End();

	camera_[currentCameraID_].Draw();
}

void CameraManager::Resize()
{
	for (auto& cam : camera_)
	{
		cam.Resize();
	}
}


void CameraManager::SetCenterTarget(Vector3 Center, int spendFrame, EaseType easetype)
{
	camera_[0].SetCenterTarget(Center, spendFrame, easetype);
}

void CameraManager::SetRotateTarget(Vector3 Center, int spendFrame, EaseType easetype)
{
	camera_[0].SetRotateTarget(Center, spendFrame, easetype);
}

void CameraManager::SetDistanceTarget(float Center, int spendFrame, EaseType easetype)
{
	camera_[0].SetDistanceTarget(Center, spendFrame, easetype);
}


void CameraManager::StartShake(float intensity, float duration, float frequency)
{
	camera_[currentCameraID_].StartShake(intensity, duration, frequency);
}

bool CameraManager::IsShaking()
{
	return camera_[currentCameraID_].IsShaking();
}

void CameraManager::StopShake()
{
	camera_[currentCameraID_].StopShake();
}


Vector3 CameraManager::GetCenter(const std::string name) const
{
	for (const auto& cam : camera_)
	{
		if (cam.name_ == name)
		{
			return cam.center_;
		}
	}

	return Vector3{};
}

Vector3 CameraManager::GetTranslate(const std::string name) const
{
	for (const auto& cam : camera_)
	{
		if (cam.name_ == name)
		{
			return cam.GetTransform().translate;
		}
	}

	return Vector3{};
}

Matrix4x4 CameraManager::GetViewProjectionMatrix(const std::string name) const
{
	for (const auto& cam : camera_)
	{
		if (cam.name_ == name)
		{
			return cam.GetViewProjectionMatrix();
		}
	}

	return Matrix4x4{};
}

float CameraManager::GetDistance(const std::string name) const
{
	for (const auto& cam : camera_)
	{
		if (cam.name_ == name)
		{
			return cam.distance_;
		}
	}

	return float{};
}


Vector3 CameraManager::GetCurrentCenter() const
{
	return camera_[currentCameraID_].center_;
}

Vector3 CameraManager::GetCurrentTranslate() const
{
	return camera_[currentCameraID_].GetTransform().translate;
}

Matrix4x4 CameraManager::GetCurrentViewProjectionMatrix() const
{
	return camera_[currentCameraID_].GetViewProjectionMatrix();
}

float CameraManager::GetCurrentDistance() const
{
	return camera_[currentCameraID_].distance_;
}


bool CameraManager::InCamera(const AABB& aabb)
{
	return camera_[currentCameraID_].InFrustum(aabb);
}

void CameraManager::ToggleCameraMode()
{
	currentCameraID_++;
	if (currentCameraID_ >= static_cast<int>(camera_.size()))
	{
		currentCameraID_ = 0;
	}
}