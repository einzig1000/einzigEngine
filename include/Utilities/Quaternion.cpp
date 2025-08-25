#include "Utilities/Quaternion.h"
#include "Utilities/functions.h"
#include <numbers>


Vector3 Quaternion::GetVector() const { return { this->x, this->y, this->z }; }
void Quaternion::Identity() { this->x = 0.0f; this->y = 0.0f; this->z = 0.0f; this->w = 1.0f; }


// 単位クォータニオンの生成
Quaternion Quaternion::MakeIdentity() { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }
// 任意軸回転を表すクォータニオンの生成
Quaternion Quaternion::MakeAxisAngle(const Vector3& axis, const float& angle)
{
	float halfAngle = angle / 2.0f;
	float sin = std::sin(halfAngle);
	float cos = std::cos(halfAngle);
	Vector3 normalizedAxis = axis.Normalized();

	return Quaternion(normalizedAxis.x * sin, normalizedAxis.y * sin, normalizedAxis.z * sin, cos);
}

// オイラー角からのクォータニオン生成
Quaternion Quaternion::MakeFromEulerAngles(const Vector3& euler)
{
	//// ラジアンに変換
	//Vector3 radEuler = {
	//	ToRadian(euler.x),
	//	ToRadian(euler.y),
	//	ToRadian(euler.z)
	//};
	//
	//// X, Y, Z軸周りの回転クォータニオンを生成
	//Quaternion qX = MakeAxisAngle({ 1.0f, 0.0f, 0.0f }, radEuler.x);
	//Quaternion qY = MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, radEuler.y);
	//Quaternion qZ = MakeAxisAngle({ 0.0f, 0.0f, 1.0f }, radEuler.z);

	// X, Y, Z軸周りの回転クォータニオンを生成
	Quaternion qX = MakeAxisAngle({ 1.0f, 0.0f, 0.0f }, euler.x);
	Quaternion qY = MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, euler.y);
	Quaternion qZ = MakeAxisAngle({ 0.0f, 0.0f, 1.0f }, euler.z);

	// Z -> X -> Y の順で回転を合成
	return qY * qX * qZ;
}

// 2つのベクトルの間の回転を表すクォータニオンを生成
Quaternion Quaternion::MakeFromToRotation(const Vector3& from, const Vector3& to)
{
	// 入力ベクトルを正規化
	Vector3 normalizedFrom = from.Normalized();
	Vector3 normalizedTo = to.Normalized();

	// 2つのベクトルがなす角度のコサインを計算
	float dotProduct = normalizedFrom.Dot(normalizedTo);

	// ベクトルがほぼ同じ方向の場合、回転は不要
	if (dotProduct >= 1.0f - FLT_EPSILON)
	{
		return Quaternion::MakeIdentity();
	}
	// ベクトルがほぼ反対方向の場合、180度回転が必要
	else if (dotProduct <= -1.0f + FLT_EPSILON)
	{
		// 回転軸を見つける
		Vector3 rotationAxis = Vector3(0.0f, 1.0f, 0.0f).Cross(normalizedFrom);
		if (rotationAxis.LengthSq() < FLT_EPSILON)
		{
			rotationAxis = Vector3(0.0f, 0.0f, 1.0f).Cross(normalizedFrom);
		}

		// 正規化して180度回転のクォータニオンを生成
		return Quaternion::MakeAxisAngle(rotationAxis.Normalized(), std::numbers::pi);
	}
	else
	{
		// 軸と角度からクォータニオンを計算
		Vector3 rotationAxis = normalizedFrom.Cross(normalizedTo);
		float angle = acosf(dotProduct);
		return Quaternion::MakeAxisAngle(rotationAxis.Normalized(), angle);
	}
}

Quaternion Quaternion::LookRotation(const Vector3& forward, const Vector3& up)
{
	// forward と up ベクトルを正規化
	Vector3 normalizedForward = forward.Normalized();
	Vector3 normalizedUp = up.Normalized();

	// forward と up が平行かどうかチェック
	if (abs(normalizedForward.Dot(normalizedUp)) > 0.999f)
	{
		// 平行な場合、LookRotationは失敗するため、代わりにデフォルトの回転を返す
		// (例: forwardをZ軸に、upをY軸に合わせる)
		return Quaternion::MakeFromToRotation(Vector3(0, 0, 1), normalizedForward);
	}

	// 軸を構築
	// 1. 新しい右ベクトル (right) を計算
	Vector3 right = normalizedUp.Cross(normalizedForward).Normalized();

	// 2. 新しい上ベクトル (up) を計算
	Vector3 newUp = normalizedForward.Cross(right);

	// 3. 回転行列を作成
	Matrix4x4 lookAtMatrix;
	lookAtMatrix.m[0][0] = right.x;     lookAtMatrix.m[0][1] = right.y;     lookAtMatrix.m[0][2] = right.z;
	lookAtMatrix.m[1][0] = newUp.x;     lookAtMatrix.m[1][1] = newUp.y;     lookAtMatrix.m[1][2] = newUp.z;
	lookAtMatrix.m[2][0] = normalizedForward.x; lookAtMatrix.m[2][1] = normalizedForward.y; lookAtMatrix.m[2][2] = normalizedForward.z;

	// 回転行列からクォータニオンを生成
	return Quaternion::MakeFromRotationMatrix(lookAtMatrix);
}

// 回転行列からのクォータニオン生成
Quaternion Quaternion::MakeFromRotationMatrix(const Matrix4x4& mat)
{
	float trace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
	if (trace > 0.0f)
	{
		float s = std::sqrt(trace + 1.0f) * 2.0f;
		float invS = 1.0f / s;
		return Quaternion(
			(mat.m[1][2] - mat.m[2][1]) * invS,
			(mat.m[2][0] - mat.m[0][2]) * invS,
			(mat.m[0][1] - mat.m[1][0]) * invS,
			s * 0.25f
		);
	}
	else if (mat.m[0][0] > mat.m[1][1] && mat.m[0][0] > mat.m[2][2])
	{
		float s = std::sqrt(mat.m[0][0] - mat.m[1][1] - mat.m[2][2] + 1.0f) * 2.0f;
		float invS = 1.0f / s;
		return Quaternion(
			s * 0.25f,
			(mat.m[0][1] + mat.m[1][0]) * invS,
			(mat.m[0][2] + mat.m[2][0]) * invS,
			(mat.m[1][2] - mat.m[2][1]) * invS
		);
	}
	else if (mat.m[1][1] > mat.m[2][2])
	{
		float s = std::sqrt(mat.m[1][1] - mat.m[0][0] - mat.m[2][2] + 1.0f) * 2.0f;
		float invS = 1.0f / s;
		return Quaternion(
			(mat.m[0][1] + mat.m[1][0]) * invS,
			s * 0.25f,
			(mat.m[1][2] + mat.m[2][1]) * invS,
			(mat.m[2][0] - mat.m[0][2]) * invS
		);
	}
	else
	{
		float s = std::sqrt(mat.m[2][2] - mat.m[0][0] - mat.m[1][1] + 1.0f) * 2.0f;
		float invS = 1.0f / s;
		return Quaternion(
			(mat.m[0][2] + mat.m[2][0]) * invS,
			(mat.m[1][2] + mat.m[2][1]) * invS,
			s * 0.25f,
			(mat.m[0][1] - mat.m[1][0]) * invS
		);
	}
}

// 積
Quaternion Quaternion::Multiply(const Quaternion& other) const
{
	return Quaternion(
		this->w * other.x + this->x * other.w + this->y * other.z - this->z * other.y,
		this->w * other.y - this->x * other.z + this->y * other.w + this->z * other.x,
		this->w * other.z + this->x * other.y - this->y * other.x + this->z * other.w,
		this->w * other.w - this->x * other.x - this->y * other.y - this->z * other.z
	);
}

// 共役クォータニオン
Quaternion Quaternion::Conjugate() const { return Quaternion(-this->x, -this->y, -this->z, this->w); }

// ノルム
float Quaternion::Norm() const { return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w); }

// 正規化
Quaternion Quaternion::Normalize() const
{
	float length = this->Norm();
	if (length == 0.0f) return *this;
	return Quaternion(this->x / length, this->y / length, this->z / length, this->w / length);
}

// 逆クォータニオン
Quaternion Quaternion::Inverse() const
{
	float norm = this->Norm();
	if (norm == 0.0f) return *this;
	float invNorm = 1.0f / (norm * norm);
	Quaternion conjugate = this->Conjugate();
	return Quaternion(conjugate.x * invNorm, conjugate.y * invNorm, conjugate.z * invNorm, conjugate.w * invNorm);
}

// クォータニオンから回転行列を作成する
Matrix4x4 Quaternion::MakeRotateMatrix() const
{
	Matrix4x4 result;
	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float xw = x * w;
	float yw = y * w;
	float zw = z * w;

	result.m[0][0] = 1.0f - (2.0f * yy) - (2.0f * zz);
	result.m[0][1] = (2.0f * xy) + (2.0f * zw);
	result.m[0][2] = (2.0f * xz) - (2.0f * yw);
	result.m[0][3] = 0.0f;

	result.m[1][0] = (2.0f * xy) - (2.0f * zw);
	result.m[1][1] = 1.0f - (2.0f * xx) - (2.0f * zz);
	result.m[1][2] = (2.0f * yz) + (2.0f * xw);
	result.m[1][3] = 0.0f;

	result.m[2][0] = (2.0f * xz) + (2.0f * yw);
	result.m[2][1] = (2.0f * yz) - (2.0f * xw);
	result.m[2][2] = 1.0f - (2.0f * xx) - (2.0f * yy);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

//// Lerp
//Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, const float& t)
//{
//	return q1 * (1.0f - t) + q2 * t;
//}
//
//// Slerp
//Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, const float& t)
//{
//	// 内積
//	float dot = q1.GetVector().Dot(q2.GetVector()) + q1.w * q2.w;
//	// 補間しすぎを防ぐため、逆方向の場合の処理
//	Quaternion q2_ = q2;
//	if (dot < 0.0f)
//	{
//		dot = -dot;
//		q2_ = q2_ * -1.0f;
//	}
//
//	// なす角を求める
//	float theta = std::acos(dot);
//	float sinTheta = std::sin(theta);
//	float scale1 = std::sin((1.0f - t) * theta) / sinTheta;
//	float scale2 = std::sin(t * theta) / sinTheta;
//	return q1 * scale1 + q2_ * scale2;
//}
//
//Vector3 RotateVector(const Vector3& vec, const Quaternion& quaternion)
//{
//	Quaternion inv = quaternion.Conjugate().Normalize();
//	return (quaternion * Quaternion(vec.x, vec.y, vec.z, 0.0f) * inv).GetVector();
//}