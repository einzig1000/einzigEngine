#pragma once
#include "definition/definition.h"

class Quaternion
{
public:
    float x, y, z, w;

    // デフォルトコンストラクタ
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

    // コンストラクタ
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Vector3 GetVector() const;
    void Identity();


	// 単位クォータニオンの生成
	static Quaternion MakeIdentity();
	// 任意軸回転を表すクォータニオンの生成
	static Quaternion MakeAxisAngle(const Vector3& axis, const float& angle);
	// オイラー角からのクォータニオン生成
	static Quaternion MakeFromEulerAngles(const Vector3& euler);
	// 2つのベクトルの間の回転を表すクォータニオンを生成
	static Quaternion MakeFromToRotation(const Vector3& from, const Vector3& to);
	// 2つのベクトルの間の回転を表すクォータニオンを生成
	static Quaternion LookRotation(const Vector3& forward, const Vector3& up);
	// 回転行列からのクォータニオン生成
	static Quaternion MakeFromRotationMatrix(const Matrix4x4& mat);
	// 度単位のオイラー角から生成
	static Quaternion FromEulerDegrees(const Vector3& eulerDeg); 
	// 回転成分を抽出してクォータニオンにする
	static Quaternion ExtractRotationFromMatrix(const Matrix4x4& mat);
	// クォータニオンをラジアン単位のオイラー角に分解
	Vector3 ToEulerRadians() const;
	// クォータニオンを度単位のオイラー角に分解
	Vector3 ToEulerDegrees() const;
	// 任意の始点と終点からLookAtクォータニオンを生成
	static Quaternion LookAt(const Vector3& eye, const Vector3& target, const Vector3& up = { 0.0f, 1.0f, 0.0f });


	// 積
	Quaternion Multiply(const Quaternion& other) const;
	// 共役クォータニオン
	Quaternion Conjugate() const;
	// ノルム
	float Norm() const;
	// 正規化
	Quaternion Normalize() const;
	// 逆クォータニオン
	Quaternion Inverse() const;

	// スカラー倍
	Quaternion operator*(const float& scalar) const
	{
		return Quaternion(this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar);
	}

	// クォータニオンの積
	Quaternion operator*(const Quaternion& other) const
	{
		return this->Multiply(other);
	}

	// ベクトルの回転
	Vector3 operator*(const Vector3& vec) const
	{
		Quaternion p(vec.x, vec.y, vec.z, 0.0f);
		Quaternion inv = this->Inverse();
		Quaternion rotated = (*this) * p * inv;
		return rotated.GetVector();
	}

	// クォータニオンから回転行列を作成する関数
	Matrix4x4 MakeRotateMatrix() const;

	// Lerp
	Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, const float& t);
	// Slerp
	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, const float& t);

	Vector3 RotateVector(const Vector3& vec, const Quaternion& quaternion);
};