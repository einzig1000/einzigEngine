#pragma once

#include "definition.h"

// DirectXTexやD3D12拡張
#include "externals/DirectXTex/d3dx12.h"

// Windows API
#include <Windows.h>
#include <sstream>


// DXC API
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler")

// マクロ定義
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


template <typename T>
constexpr const T& my_min(const T& a, const T& b)
{
    return (a < b) ? a : b;
}

template <typename T>
constexpr const T& my_max(const T& a, const T& b)
{
    return (a > b) ? a : b;
}

/// <summary>
/// Vector3の長さ
/// </summary>
/// <param name="v">Vector3</param>
/// <returns>vの長さ</returns>
//float Length(const Vector3& v);
//
// 3頂点から法線ベクトルを計算し、正規化して返す
Vector3 CalculateNormal(const Vector4& v0, const Vector4& v1, const Vector4& v2);

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

Vector4 Transform(const Vector4& v, const Matrix4x4& m);



bool IsCollision(const Sphere& s1, const Sphere& s2);
bool IsCollision(const Sphere& s, const Plane& p);
bool IsCollision(const Segment& s, const Plane& p);
bool IsCollision(const Segment& s, const Triangle& t);
bool IsCollision(const Ray& r, const Plane& p);
bool IsCollision(const Ray& r, const AABB& aabb);
bool IsCollision(const Ray& r, const Triangle& t);

bool IsCollision(const Ray& ray, const AABB& aabb, const std::vector<VertexData>& vertices, const Matrix4x4& worldMatrix);

// ARGBをRGBA
Vector4 ConvertARGBtoRGBA(const Vector4& argb);
// int型のカラーをVector4型に
Vector4 ConvertUintToVector4(uint32_t color);

/// <summary>
/// 球体の頂点データを生成する関数
/// </summary>
/// <param name="vertexData">頂点データを格納する配列</param>
/// <param name="kSubdivision">球体の分割数</param>
void CreateSphere(VertexData* vertexData, uint32_t kSubdivision);

/// <summary>
/// UTF-8 文字列をワイド文字列 (UTF-16) に変換する関数
/// </summary>
/// <param name="str">変換する UTF-8 文字列</param>
/// <returns>変換されたワイド文字列</returns>
std::wstring ConvertString(const std::string& str);

/// <summary>
/// ワイド文字列 (UTF-16) を UTF-8 文字列に変換する関数
/// </summary>
/// <param name="str">変換するワイド文字列</param>
/// <returns>変換された UTF-8 文字列</returns>
std::string ConvertString(const std::wstring& str);

/// <summary>
/// D3D12_RESOURCE_STATES を文字列に変換する関数
/// </summary>
/// <param name="state">リソースの状態 (D3D12_RESOURCE_STATES)</param>
/// <returns>リソース状態を表す文字列</returns>
std::string ResourceStateToString(D3D12_RESOURCE_STATES state);

/// <summary>
/// デバッグ用のログを出力する関数
/// </summary>
/// <param name="message">出力するメッセージ</param>
void Log(const std::string& message);

/// <summary>
/// デバッグ用のログを出力する関数 (Vector4型の値を含む)
/// </summary>
/// <param name="message">出力するメッセージ</param>
/// <param name="vector">ログに出力する Vector4</param>
void Log(const std::string& message, const Vector4& vector);

/// <summary>
/// デバッグ用のログを出力する関数 (Matrix4x4型の値を含む)
/// </summary>
/// <param name="message">出力するメッセージ</param>
/// <param name="matrix">ログに出力する Matrix4x4</param>
void Log(const std::string& message, const Matrix4x4& matrix);

/// <summary>
/// デバッグ用のログを出力する関数 (D3D12_RESOURCE_BARRIER の状態を含む)
/// </summary>
/// <param name="message">出力するメッセージ</param>
/// <param name="barrier">ログに出力するリソースバリア</param>
void Log(const std::string& message, const D3D12_RESOURCE_BARRIER& barrier);

/// <summary>
/// デバッグ用のログを出力する関数 (D3D12_ROOT_SIGNATURE_DESC の状態を含む)
/// </summary>
/// <param name="desc">ログに出力するルートシグネチャの記述子</param>
void Log(const D3D12_ROOT_SIGNATURE_DESC& desc);

/// <summary>
/// ログをファイルに書き出す関数
/// </summary>
/// <param name="os">出力先のファイルストリーム</param>
/// <param name="message">出力するメッセージ</param>
void Log(std::ofstream& os, const std::string& message);


// printf形式のLog関数
void Log(const char* format, ...);

// std::string を受け取る Log 関数 (functions.cpp で定義される)
void Log(const std::string& message);













/// <summary>
/// クラッシュ時にミニダンプを生成する関数
/// </summary>
/// <param name="exception">例外情報</param>
/// <returns>例外ハンドラの実行結果</returns>
LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);

/// <summary>
/// バッファリソースを作成する関数
/// </summary>
/// <param name="device">DirectX 12 デバイス</param>
/// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
/// <returns>作成されたバッファリソース</returns>
ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

/// <summary>
/// HLSL シェーダーをコンパイルする関数
/// </summary>
/// <param name="filePath">シェーダーファイルのパス</param>
/// <param name="profile">コンパイルに使用するプロファイル</param>
/// <param name="dxcUtils">DXC ユーティリティ</param>
/// <param name="dxcCompiler">DXC コンパイラ</param>
/// <param name="includeHandler">インクルードハンドラ</param>
/// <returns>コンパイルされたシェーダーバイナリ</returns>
IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

/// <summary>
/// 指定されたタイプと数のディスクリプタヒープを作成する関数
/// </summary>
/// <param name="device">DirectX 12 デバイス</param>
/// <param name="heapType">ディスクリプタヒープのタイプ</param>
/// <param name="numDescriptors">ディスクリプタの数</param>
/// <param name="shaderVisible">シェーダーからアクセス可能かどうか</param>
/// <returns>作成されたディスクリプタヒープ</returns>
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);


/// <summary>
/// テクスチャファイルを読み込み、ミップマップを生成する関数
/// </summary>
/// <param name="filePath">テクスチャファイルのパス</param>
/// <returns>読み込まれたテクスチャデータ</returns>
///DirectX::ScratchImage LoadTexture(const std::string& filePath);

/// <summary>
/// テクスチャのメタデータを基に DirectX 12 のテクスチャリソースを作成する関数
/// </summary>
/// <param name="device">DirectX 12 デバイス</param>
/// <param name="metadata">テクスチャのメタデータ</param>
/// <returns>作成されたテクスチャリソース</returns>
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

/// <summary>
/// テクスチャデータを GPU にアップロードする関数
/// </summary>
/// <param name="texture">アップロード先のテクスチャリソース</param>
/// <param name="mipImages">ミップマップデータ</param>
/// <param name="device">DirectX 12 デバイス</param>
/// <param name="commandList">コマンドリスト</param>
/// <returns>アップロードに使用した中間リソース</returns>
[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

/// <summary>
/// 深度ステンシルテクスチャリソースを作成する関数
/// </summary>
/// <param name="device">DirectX 12 デバイス</param>
/// <param name="width">テクスチャの幅</param>
/// <param name="height">テクスチャの高さ</param>
/// <returns>作成された深度ステンシルテクスチャリソース</returns>
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);


/// <summary>
/// 指定されたインデックスの CPU ディスクリプタハンドルを取得する関数
/// </summary>
/// <param name="descriptorHeap">ディスクリプタヒープ</param>
/// <param name="descriptorSize">ディスクリプタのサイズ</param>
/// <param name="index">インデックス</param>
/// <returns>取得した CPU ディスクリプタハンドル</returns>
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

/// <summary>
/// 指定されたインデックスの GPU ディスクリプタハンドルを取得する関数
/// </summary>
/// <param name="descriptorHeap">ディスクリプタヒープ</param>
/// <param name="descriptorSize">ディスクリプタのサイズ</param>
/// <param name="index">インデックス</param>
/// <returns>取得した GPU ディスクリプタハンドル</returns>
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

/// <summary>
/// MTL ファイルを読み込み、マテリアルデータを構築する関数
/// </summary>
/// <param name="directoryPath">MTL ファイルのディレクトリパス</param>
/// <param name="filename">MTL ファイル名</param>
/// <returns>構築されたマテリアルデータ</returns>
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

/// <summary>
/// OBJ ファイルを読み込み、モデルデータを構築する関数
/// </summary>
/// <param name="directoryPath">OBJ ファイルのディレクトリパス</param>
/// <param name="filename">OBJ ファイル名</param>
/// <returns>構築されたモデルデータ</returns>
ModelData LoadOBJFile(const std::string& directoryPath, const std::string& filename);
