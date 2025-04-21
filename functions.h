#pragma once
#include "definition.h"

#pragma region Vector3

/// <summary>
/// Vector3の足し算
/// </summary>
/// <param name="v1">Vector3</param>
/// <param name="v2">Vector3</param>
/// <returns>v1とv2の和</returns>
Vector3 Add(const Vector3& v1, const Vector3& v2);

/// <summary>
/// Vector3の引き算
/// </summary>
/// <param name="v1">Vector3</param>
/// <param name="v2">Vector3</param>
/// <returns>v1とv2の差</returns>
Vector3 Sub(const Vector3& v1, const Vector3& v2);

/// <summary>
/// Vector3の掛け算
/// </summary>
/// <param name="v1">Vector3</param>
/// <param name="v2">Vector3</param>
/// <returns>v1とv2の積</returns>
Vector3 Mul(float scalar, const Vector3& v);

/// <summary>
/// Vector3の内積
/// </summary>
/// <param name="v1">Vector3</param>
/// <param name="v2">Vector3</param>
/// <returns>v1とv2の内積</returns>
float DotProduct(const Vector3& v1, const Vector3& v2);

/// <summary>
/// Vector3の外積
/// </summary>
/// <param name="v1">Vector3</param>
/// <param name="v2">Vector3</param>
/// <returns>v1とv2の外積</returns>
Vector3 CrossProduct(const Vector3& v1, const Vector3& v2);

/// <summary>
/// Vector3の長さ
/// </summary>
/// <param name="v">Vector3</param>
/// <returns>vの長さ</returns>
float Length(const Vector3& v);

/// <summary>
/// 正規化したVector3
/// </summary>
/// <param name="v">Vector3</param>
/// <returns>正規化したv</returns>
Vector3 Normalize(const Vector3& v);

/// <summary>
/// Vector3の表示
/// </summary>
/// <param name="x">座標ｘ</param>
/// <param name="y">座標ｙ</param>
/// <param name="vector">表示したいVector3</param>
/// <param name="label">表示したいコメント</param>
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);

/// <summary>
/// Vector3の座標変換
/// </summary>
/// <param name="vector">変換したいベクトル</param>
/// <param name="matrix">適用したいマトリックス</param>
/// <returns>座標変換されたvector</returns>
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

#pragma endregion


#pragma region Matrix4x4
/// <summary>
/// Matrix4x4の足し算
/// </summary>
/// <param name="m1">Matrix4x4</param>
/// <param name="m2">Matrix4x4</param>
/// <returns>"m1"と"m2"の和</returns>
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// Matrix4x4の引き算
/// </summary>
/// <param name="m1">Matrix4x4</param>
/// <param name="m2">Matrix4x4</param>
/// <returns>"m1"と"m2"の差</returns>
Matrix4x4 Sub(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// Matrix4x4の掛け算
/// </summary>
/// <param name="m1">Matrix4x4</param>
/// <param name="m2">Matrix4x4</param>
/// <returns>"m1"と"m2"の積</returns>
Matrix4x4 Mul(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// Matrix4x4の逆行列
/// </summary>
/// <param name="m">Matrix4x4</param>
/// <returns>"m"の逆行列</returns>
Matrix4x4 Inverse(const Matrix4x4& m);

/// <summary>
/// Matrix4x4の転置行列
/// </summary>
/// <param name="m">Matrix4x4</param>
/// <returns>"m"の転置行列</returns>
Matrix4x4 Transpose(const Matrix4x4& m);

/// <summary>
/// Matrix4x4の単位行列作成関数
/// </summary>
/// <returns>Matrix4x4の単位行列</returns>
Matrix4x4 MakeIdentity4x4();

/// <summary>
/// Vector3の平行移動行列
/// </summary>
/// <param name="translate">移動させたいベクトル量</param>
/// <returns>"translate"分移動させられる平行移動行列</returns>
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

/// <summary>
/// Vector3の拡縮行列
/// </summary>
/// <param name="scale">拡縮したいベクトル量</param>
/// <returns>"scale"分拡縮できる拡縮行列</returns>
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);

// Ｘ軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

// Ｙ軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

// Ｚ軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

// ３次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// 透視投影行列（透視変換行列）
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// ビューポート変換
Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minD, float maxD);

#pragma endregion