#include "Utilities/functions.h"
#include "definition/definition.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <format>
#include <algorithm>
#include <cstdio>
#include <cstdarg>

#include <d3d12.h>
#include <wrl.h>

#define NOMINMAX
#include <windows.h>
#include <DbgHelp.h>
#include <strsafe.h>
#include "enum.h"
#include "Game.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "Dbghelp.lib")


#ifdef _MSC_VER
#define VSNPRINTF_FUNC _vsnprintf_s
#else
#define VSNPRINTF_FUNC vsnprintf
#endif


Vector2int IndexByPosition(Vector3 pos)
{
    Vector2int index;
    // ブロック中心座標からインデックスを計算
    index.x = static_cast<int>(std::round(-pos.x / BLOCK_WIDTH));
    index.y = static_cast<int>(std::round(-pos.z / BLOCK_HEIGHT));
    // 範囲外の値を制限
    index.x = std::clamp(index.x, 0, MAP_WIDTH - 1);
    index.y = std::clamp(index.y, 0, MAP_HEIGHT - 1);
    return index;
}
Vector3 PositionByIndex(Vector2int index)
{
    Vector3 pos;

    pos.x = -static_cast<float>(index.x) * BLOCK_WIDTH;
    pos.y = 0.0f;
    pos.z = -static_cast<float>(index.y) * BLOCK_HEIGHT;

    return pos;
}




Vector3 TriangleNormal(const Vector4& v0, const Vector4& v1, const Vector4& v2)
{
    Vector3 ab = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
    Vector3 ac = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };
    return ab.Cross(ac).Normalized();
}
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix)
{
    Vector3 result{};

    // ベクトルと行列の計算
    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
    result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
    float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

    // wが0でなければ
    assert(w != 0.0f);
    result.x /= w;
    result.y /= w;
    result.z /= w;

    // 計算結果をVector3型で返す
    return result;
}
Vector4 Transform(const Vector4& v, const Matrix4x4& m)
{
    Vector4 result;
    result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
    result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
    result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
    result.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
    return result;
}


#pragma region collision

bool IsCollision(const Sphere& s1, const Sphere& s2)
{
    Vector3 gappoint;
    gappoint.x = s1.center.x - s2.center.x;
    gappoint.y = s1.center.y - s2.center.y;
    gappoint.z = s1.center.z - s2.center.z;

    float gap = gappoint.Length();

    float i = s1.radius + s2.radius;

    if (i < gap)return false;
    else return true;
}

bool IsCollision(const Sphere& s, const Plane& p)
{
    // 球の中心から平面までの距離を計算
    float dist = s.center.Dot(p.normal) - p.distance;
    // 距離の絶対値が半径以下なら衝突
    return std::abs(dist) <= s.radius;
}

bool IsCollision(const Segment& s, const Plane& p)
{
    // 線分の始点と終点
    const Vector3& start = s.origin;
    const Vector3& end = (s.origin + s.diff);

    // 始点と終点が平面のどちら側にあるかを判定　この数字が０になると、平面上にあるということになる
    float distStart = start.Dot(p.normal) - p.distance;
    float distEnd = end.Dot(p.normal) - p.distance;

    // 始点と終点が平面の表裏にあるなら（distStartとdistEndの組み合わせが０以下と以上）交差してる
    if (distStart * distEnd <= 0.0f)
    {
        return true;
    }
    return false;
}

bool IsCollision(const Segment& s, const Triangle& t)
{
    // 三角形の法線と平面の距離を求める
    Vector3 edge1 = (t.vertices[1] - t.vertices[0]);
    Vector3 edge2 = (t.vertices[2] - t.vertices[0]);
    Vector3 normal = (edge1.Cross(edge2)).Normalized();
    float distance = normal.Dot(t.vertices[0]);

    // １，線と三角形の存在する平面の衝突判定
    if (!IsCollision(s, Plane{ normal, distance }))
    {
        return false;
    }

    // bool IsCollision(const Segment & s, const Plane & p)より
    // 線分の始点と終点
    Vector3 start = s.origin;
    Vector3 end = (s.origin + s.diff);

    // 線分と平面の交点を求める
    float distStart = start.Dot(normal) - distance;
    float distEnd = end.Dot(normal) - distance;
    float tParam = distStart / (distStart - distEnd);
    // 衝突点
    Vector3 intersect = (start + ((end - start) * tParam));
    //Vector3 intersect = Add(start, Mul(tParam, Sub(end, start)));

    // 各辺と交点のクロス積で判定
    bool allSame = true;
    float sign = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        // 始点
        Vector3 v0 = t.vertices[i];
        // 終点
        Vector3 v1 = t.vertices[(i + 1) % 3];
        // 始点と終点のベクトル
        Vector3 edge = (v1 - v0);
        // 始点と衝突点のベクトル
        Vector3 toP = (intersect - v0);
        // 上記２つのクロス積
        Vector3 cross = (edge.Cross(toP));
        // 三角形の法線とクロス積の内積
        float dot = normal.Dot(cross);
        // 1つ目の三角形の向きを基準にして2,3つ目の向きと比較する
        if (i == 0)
        {
            sign = dot;
        }
        else
        {
            // 向きの不一致が起きた
            if (sign * dot < 0.0f)
            {
                allSame = false;
                break;
            }
        }
    }
    return allSame;
}

bool IsCollision(const Ray& r, const Plane& p)
{
    // 線分の始点と方向
    const Vector3& start = r.origin;
    const Vector3& dir = r.diff;

    // 面の法線と原点らの距離
    const Vector3& normal = p.normal;
    float distance = p.distance;

    // レイの方向と平面法線の内積
    float denom = dir.Dot(p.normal);

    // レイが平面と平行なら衝突しない
    if (std::abs(denom) < 1e-6f)
    {
        return false;
    }

    // レイの始点が平面より手前にあるか
    float t = (p.distance - (start.Dot(normal))) / denom;

    if (t < 0.0f)
    {
        return false;
    }

    return true;
}

bool IsCollision(const Ray& r, const AABB& aabb)
{
    float tmin = (aabb.min.x - r.origin.x) / r.diff.x;
    float tmax = (aabb.max.x - r.origin.x) / r.diff.x;
    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (aabb.min.y - r.origin.y) / r.diff.y;
    float tymax = (aabb.max.y - r.origin.y) / r.diff.y;
    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (aabb.min.z - r.origin.z) / r.diff.z;
    float tzmax = (aabb.max.z - r.origin.z) / r.diff.z;
    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    return true;
}

bool IsCollision(const Ray& r, const Triangle& t)
{
    // 三角形の法線と平面の距離を求める
    Vector3 edge1 = (t.vertices[1] - t.vertices[0]);
    Vector3 edge2 = (t.vertices[2] - t.vertices[0]);
    Vector3 normal = (edge1.Cross(edge2)).Normalized();
    float distance = (normal.Dot(t.vertices[0]));

    // １，線と三角形の存在する平面の衝突判定
    if (!IsCollision(r, Plane{ normal, distance }))
    {
        return false;
    }

    // 線分の始点と方向
    const Vector3& start = r.origin;
    const Vector3& dir = r.diff;

    // 線分と平面の交点を求める
    float denom = (dir.Dot(normal));
    float tParam = (distance - (start.Dot(normal))) / denom;
    // 衝突点
    Vector3 intersect = (start + (dir * tParam));

    // 各辺と交点のクロス積で判定
    bool allSame = true;
    float sign = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        // 始点
        Vector3 v0 = t.vertices[i];
        // 終点
        Vector3 v1 = t.vertices[(i + 1) % 3];
        // 始点と終点のベクトル
        Vector3 edge = (v1 - v0);
        // 始点と衝突点のベクトル
        Vector3 toP = (intersect - v0);
        // 上記２つのクロス積
        Vector3 cross = (edge.Cross(toP));
        // 三角形の法線とクロス積の内積
        float dot = (normal.Dot(cross));
        // 1つ目の三角形の向きを基準にして2,3つ目の向きと比較する
        if (i == 0)
        {
            sign = dot;
        }
        else
        {
            // 向きの不一致が起きた
            if (sign * dot < 1e-6f)
            {
                allSame = false;
                break;
            }
        }
    }
    return allSame;
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2)
{
    // 各軸で分離していれば衝突していない
    if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x) return false;
    if (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y) return false;
    if (aabb1.max.z < aabb2.min.z || aabb1.min.z > aabb2.max.z) return false;
    // どの軸でも分離していなければ衝突
    return true;
}

bool IsCollision(const AABB& aabb, const Sphere& s)
{
    // 最近接点を求めるf
    Vector3 closest{};
    // 各軸ごとにAABBの範囲内にクランプ
    closest.x = my_max(aabb.min.x, my_min(s.center.x, aabb.max.x));
    closest.y = my_max(aabb.min.y, my_min(s.center.y, aabb.max.y));
    closest.z = my_max(aabb.min.z, my_min(s.center.z, aabb.max.z));

    // 最近接点と球の中心の距離を計算
    Vector3 diff = s.center - closest;
    float dist = diff.Length();

    // 距離が半径以下なら衝突
    return dist <= s.radius;
}

bool IsCollision(const AABB& aabb, const Segment& s)
{
    // 線分の始点と終点
    Vector3 start = s.origin;
    Vector3 end = s.origin + s.diff;

    // min軸と線分の交点の{線分の割合}とmax軸ver
    float tmin = 0.0f;
    float tmax = 1.0f;

    // x,y,z三回の計算をforループで行う悪魔的所業
    for (int i = 0; i < 3; ++i)
    {
        float segStart, segEnd, boxMin, boxMax;
        // X軸から見た線分の始点と終点 ＆ AABBのmin.xとmax.x
        if (i == 0) { segStart = start.x; segEnd = end.x; boxMin = aabb.min.x; boxMax = aabb.max.x; }
        // Y軸から見た線分の始点と終点 ＆ AABBのmin.yとmax.y
        else if (i == 1) { segStart = start.y; segEnd = end.y; boxMin = aabb.min.y; boxMax = aabb.max.y; }
        // Z軸から見た線分の始点と終点 ＆ AABBのmin.zとmax.z
        else { segStart = start.z; segEnd = end.z; boxMin = aabb.min.z; boxMax = aabb.max.z; }

        // 線分の長さ
        float d = segEnd - segStart;
        // 長さが限りなく０に近い時は点として扱う
        if (std::abs(d) < 1e-6f)
        {
            // 点がmin以上max以下ではないなら当たる見込みなし
            if (segStart < boxMin || segStart > boxMax) return false;
        }
        // 点じゃなかった
        else
        {
            // dを１として tを計算
            float t1 = (boxMin - segStart) / d;
            float t2 = (boxMax - segStart) / d;
            // 線分の方向によってはmax側から入ることもあるのでt1とt2を入れ替える
            if (t1 > t2) std::swap(t1, t2);
            // tは線分の割合なので、tが小さい方がtNear,tが大きい方がtFarになる
            // 貫通している場合「Near.XorY → Near.XorY → Far.XorY → Far.XorY」の順になるはずなので
            // tNearの大きい方 < tFarの小さい方になっていれば貫通している
            tmin = my_max(tmin, t1);
            tmax = my_min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }
    return true;
}

// モデルのAABBと三角形配列で詳細判定
bool IsCollision(const Ray& ray, const std::vector<VertexData>& vertices, const AABB& aabb, const Transforms& data)
{
    // まずAABBで大まかに判定
    if (!IsCollision(ray, aabb))
    {
        return false;
    }

    // AABBに当たっていた場合のみ、三角形ごとに詳細判定
    for (size_t i = 0; i + 2 < vertices.size(); i += 3)
    {
        Triangle t;
        // 三角形の頂点をワールド座標に変換
        t.vertices[0] = Transform(
            Vector3{ vertices[i].position.x, vertices[i].position.y, vertices[i].position.z },
            data.World
        );
        t.vertices[1] = Transform(
            Vector3{ vertices[i + 1].position.x, vertices[i + 1].position.y, vertices[i + 1].position.z },
            data.World
        );
        t.vertices[2] = Transform(
            Vector3{ vertices[i + 2].position.x, vertices[i + 2].position.y, vertices[i + 2].position.z },
            data.World
        );
        //Log("Triangle[0].x : %f,Triangle[0].y : %f,Triangle[0].z : %f", t.vertices[0].x, t.vertices[0].y, t.vertices[0].z);
        //Log("Triangle[1].x : %f,Triangle[1].y : %f,Triangle[1].z : %f", t.vertices[1].x, t.vertices[1].y, t.vertices[1].z);
        //Log("Triangle[2].x : %f,Triangle[2].y : %f,Triangle[2].z : %f", t.vertices[2].x, t.vertices[2].y, t.vertices[2].z);
        //Log("Ray.origin.x : %f,Ray.origin.y : %f,Ray.origin.z : %f", ray.origin.x, ray.origin.y, ray.origin.z);
        //Log("Ray.diff.x : %f,Ray.diff.y : %f,Ray.diff.z : %f", ray.diff.x, ray.diff.y, ray.diff.z);

        if (IsCollision(ray, t))
        {
            return true; // どれか1つでも当たればtrue
        }
    }
    return false;
}





#pragma endregion

#pragma region Log

// ログを出す関数
void Log(const std::string& message)
{
    // string型からchar*型に変換した文字列
    OutputDebugStringA((message + "\n").c_str());
}
// Vector4型用のオーバーロード
void Log(const std::string& message, const Vector4& vector)
{
    Log(message);
    std::string a = std::format("x={}, y={}, z={}, w={}", vector.x, vector.y, vector.z, vector.w);
    Log(a);
}
// Matrix4x4型用のオーバーロード
void Log(const std::string& message, const Matrix4x4& matrix)
{
    Log(message);
    std::string a = ":\n";
    for (int i = 0; i < 4; ++i)
    {
        a += std::format("[{}, {}, {}, {}]\n", matrix.m[i][0], matrix.m[i][1], matrix.m[i][2], matrix.m[i][3]);
    }
    Log(a);
}
// barrier.Transitionの状態をログに出力する関数
void Log(const std::string& message, const D3D12_RESOURCE_BARRIER& barrier)
{
    Log(message);
    if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
    {
        std::string stateBefore = ResourceStateToString(barrier.Transition.StateBefore);
        std::string stateAfter = ResourceStateToString(barrier.Transition.StateAfter);
        std::string a = std::format("Barrier Transition - StateBefore: {}, StateAfter: {}", stateBefore, stateAfter);
        Log(a);
    }
    else
    {
        Log("Barrier is not of type TRANSITION.");
    }
}
// RootSignatureの状態をログに出力する関数
void Log(const D3D12_ROOT_SIGNATURE_DESC& desc)
{
    std::ostringstream oss;
    oss << "[Root Signature]\n"
        << "NumParameters: " << desc.NumParameters << "\n"
        << "NumStaticSamplers: " << desc.NumStaticSamplers << "\n"
        << "Flags: " << desc.Flags << "\n";

    Log(oss.str());

    for (UINT i = 0; i < desc.NumParameters; ++i)
    {
        const auto& param = desc.pParameters[i];
        oss.str(""); // バッファをクリア
        oss.clear(); // 状態をリセット
        oss << "[Root Parameter " << i << "]\n"
            << "Type: " << param.ParameterType << "\n"
            << "ShaderVisibility: " << param.ShaderVisibility << "\n";
        Log(oss.str());

        if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV ||
            param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV ||
            param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
        {
            oss.str("");
            oss.clear();
            oss << "ShaderRegister: " << param.Descriptor.ShaderRegister << "\n"
                << "RegisterSpace: " << param.Descriptor.RegisterSpace << "\n";
            Log(oss.str());
        }
        else if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            oss.str("");
            oss.clear();
            oss << "NumDescriptorRanges: " << param.DescriptorTable.NumDescriptorRanges << "\n";
            Log(oss.str());
            for (UINT j = 0; j < param.DescriptorTable.NumDescriptorRanges; ++j)
            {
                const auto& range = param.DescriptorTable.pDescriptorRanges[j];
                oss.str("");
                oss.clear();
                oss << "  [Descriptor Range " << j << "]\n"
                    << "  RangeType: " << range.RangeType << "\n"
                    << "  BaseShaderRegister: " << range.BaseShaderRegister << "\n"
                    << "  NumDescriptors: " << range.NumDescriptors << "\n"
                    << "  RegisterSpace: " << range.RegisterSpace << "\n";
                Log(oss.str());
            }
        }
    }

    for (UINT i = 0; i < desc.NumStaticSamplers; ++i)
    {
        const auto& sampler = desc.pStaticSamplers[i];
        oss.str("");
        oss.clear();
        oss << "[Static Sampler " << i << "]\n"
            << "ShaderRegister: " << sampler.ShaderRegister << "\n"
            << "Filter: " << sampler.Filter << "\n"
            << "AddressU: " << sampler.AddressU << "\n"
            << "AddressV: " << sampler.AddressV << "\n"
            << "AddressW: " << sampler.AddressW << "\n"
            << "ShaderVisibility: " << sampler.ShaderVisibility << "\n";
        Log(oss.str());
    }
}
// もはやこれしか使わないログ
void Log(const char* format, ...)
{
    // 最大バッファサイズを設定 (文字列の最大長)
    const int INITIAL_BUFFER_SIZE = 256;
    std::vector<char> charBuffer(INITIAL_BUFFER_SIZE); // char 型の動的バッファ

    va_list args;
    va_start(args, format);

    // 最初にバッファサイズを試行。BUFFER_SIZEが足りなければ、vsnprintf君は必要としているサイズをいつも返してくれる
    int written = VSNPRINTF_FUNC(charBuffer.data(), charBuffer.size(), _TRUNCATE, format, args);
    // _TRUNCATE は MSVC 固有のオプションで、バッファが足りない場合に切り詰める

    if (written < 0 || written >= charBuffer.size())
    {
        // バッファが足りなかった、またはエラーが発生した場合
        // 必要なサイズを計算してバッファをリサイズし再試行
        size_t required_size = charBuffer.size() * 2; // ジャスト必要な分あるはずだけど一応2倍にするとよいらしい
        if (written > 0)
        {
            required_size = static_cast<size_t>(written) + 1; // +1 for null terminator
        }
        charBuffer.resize(required_size);
        // va_list をリセットして再利用 (va_copy を使うのがより堅牢)
        va_end(args); // 一度終了
        va_start(args, format); // 再度開始

        written = VSNPRINTF_FUNC(charBuffer.data(), charBuffer.size(), _TRUNCATE, format, args);
    }

    va_end(args); // 可変引数リストの終了

    // ここで charBuffer.data() にフォーマットされた文字列が入っている

    // char (マルチバイト) から wchar_t (ワイド文字) に変換
    // 変換に必要なバッファサイズを取得
    int wlen = MultiByteToWideChar(CP_UTF8, 0, charBuffer.data(), written, nullptr, 0);
    if (wlen > 0)
    {
        std::vector<wchar_t> wcharBuffer(wlen + 2); // 改行とNULL終端のために+2
        MultiByteToWideChar(CP_UTF8, 0, charBuffer.data(), written, wcharBuffer.data(), wlen);

        // ワイド文字バッファに改行とNULL終端を追加
        wcharBuffer[wlen] = L'\n';
        wcharBuffer[wlen + 1] = L'\0';

        // OutputDebugStringW で出力
        OutputDebugStringW(wcharBuffer.data());
    }
    else
    {
        // 変換エラーの場合
        OutputDebugStringA("Log: Error converting multi-byte to wide-char.I can't speak Japanese hahaha Sushi\n");
        // 元の charBuffer をそのまま出力 (文字化け覚悟)
        OutputDebugStringA(charBuffer.data());
        OutputDebugStringA("\n");
    }
}
// ログをファイルに書き出す
void Log(std::ofstream& os, const std::string& message)
{
    os << message << std::endl;
    OutputDebugStringA(message.c_str());
}

#pragma endregion


Vector4 ConvertUintToVector4(uint32_t color)
{
    float r = ((color >> 24) & 0xFF) / 255.0f;
    float g = ((color >> 16) & 0xFF) / 255.0f;
    float b = ((color >> 8) & 0xFF) / 255.0f;
    float a = (color & 0xFF) / 255.0f;
    return { r, g, b, a };
}

uint32_t ConvertVector4ToUint(Vector4 color)
{
    uint32_t r = static_cast<uint32_t>(std::clamp(color.x * 255.0f, 0.0f, 255.0f));
    uint32_t g = static_cast<uint32_t>(std::clamp(color.y * 255.0f, 0.0f, 255.0f));
    uint32_t b = static_cast<uint32_t>(std::clamp(color.z * 255.0f, 0.0f, 255.0f));
    uint32_t a = static_cast<uint32_t>(std::clamp(color.w * 255.0f, 0.0f, 255.0f));
    return (r << 24) | (g << 16) | (b << 8) | a;
}

// ARGBをRGBA
Vector4 ConvertARGBtoRGBA(const Vector4& argb)
{
    return { argb.y, argb.z, argb.w, argb.x };
}

void CreateSphere(VertexData* vertexData, uint32_t kSubdivision)
{
    if (kSubdivision == 0 || vertexData == nullptr)
    {
        return;
    }

    // 経度分割１つ分の角度
    const float kLonEvery = float((2 * M_PI) / kSubdivision);
    // 緯度分割１つ分の角度
    const float kLatEvery = float(M_PI / kSubdivision);

    // 緯度の方向に分割 -π/2 ～ π/2
    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
    {
        // 現在の緯度と次の緯度
        const float lat = float(-M_PI / 2.0f + latIndex * kLatEvery);
        const float nextLat = lat + kLatEvery;

        // 経度方向に分割 0 ～ 2π
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
        {
            // 現在の経度と次の経度
            const float lon = lonIndex * kLonEvery;
            const float nextLon = (lonIndex + 1) * kLonEvery;

            // テクスチャ座標を計算（[0,1]範囲をしっかりカバー）
            const float u = float(lonIndex) / float(kSubdivision);
            const float nextU = float(lonIndex + 1) / float(kSubdivision);
            const float v = 1.0f - float(latIndex) / float(kSubdivision);
            const float nextV = 1.0f - float(latIndex + 1) / float(kSubdivision);

            // 頂点データの開始インデックス
            const uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;

            // 頂点データを設定 (三角形1)
            vertexData[start + 0].position = { std::cos(lat) * std::cos(lon), std::sin(lat), std::cos(lat) * std::sin(lon), 1.0f };
            vertexData[start + 0].texcoord = { u, v };

            vertexData[start + 2].position = { std::cos(nextLat) * std::cos(nextLon), std::sin(nextLat), std::cos(nextLat) * std::sin(nextLon), 1.0f };
            vertexData[start + 2].texcoord = { nextU, nextV };

            vertexData[start + 1].position = { std::cos(nextLat) * std::cos(lon), std::sin(nextLat), std::cos(nextLat) * std::sin(lon), 1.0f };
            vertexData[start + 1].texcoord = { u, nextV };

            // 頂点データを設定 (三角形2)
            vertexData[start + 3].position = { std::cos(lat) * std::cos(lon), std::sin(lat), std::cos(lat) * std::sin(lon), 1.0f };
            vertexData[start + 3].texcoord = { u, v };

            vertexData[start + 5].position = { std::cos(lat) * std::cos(nextLon), std::sin(lat), std::cos(lat) * std::sin(nextLon), 1.0f };
            vertexData[start + 5].texcoord = { nextU, v };

            vertexData[start + 4].position = { std::cos(nextLat) * std::cos(nextLon), std::sin(nextLat), std::cos(nextLat) * std::sin(nextLon), 1.0f };
            vertexData[start + 4].texcoord = { nextU, nextV };

            // 法線を正規化して設定
            for (int i = 0; i < 6; ++i)
            {
                Vector3 n = {
                    vertexData[start + i].position.x,
                    vertexData[start + i].position.y,
                    vertexData[start + i].position.z
                };
                vertexData[start + i].normal = (n.Normalized());
            }
        }
    }
}

// 文字列変換
std::wstring ConvertString(const std::string& str)
{
    if (str.empty())
    {
        return std::wstring();
    }


    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0)
    {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

// 文字列変換
std::string ConvertString(const std::wstring& str)
{
    if (str.empty())
    {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0)
    {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}

// D3D12_RESOURCE_STATES を文字列に変換する関数
std::string ResourceStateToString(D3D12_RESOURCE_STATES state)
{
    switch (state)
    {
    case D3D12_RESOURCE_STATE_COMMON: return "COMMON";
    case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER: return "VERTEX_AND_CONSTANT_BUFFER";
    case D3D12_RESOURCE_STATE_INDEX_BUFFER: return "INDEX_BUFFER";
    case D3D12_RESOURCE_STATE_RENDER_TARGET: return "RENDER_TARGET";
    case D3D12_RESOURCE_STATE_UNORDERED_ACCESS: return "UNORDERED_ACCESS";
    case D3D12_RESOURCE_STATE_DEPTH_WRITE: return "DEPTH_WRITE";
    case D3D12_RESOURCE_STATE_DEPTH_READ: return "DEPTH_READ";
    case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE: return "NON_PIXEL_SHADER_RESOURCE";
    case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE: return "PIXEL_SHADER_RESOURCE";
    case D3D12_RESOURCE_STATE_STREAM_OUT: return "STREAM_OUT";
    case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT: return "INDIRECT_ARGUMENT";
    case D3D12_RESOURCE_STATE_COPY_DEST: return "COPY_DEST";
    case D3D12_RESOURCE_STATE_COPY_SOURCE: return "COPY_SOURCE";
    case D3D12_RESOURCE_STATE_RESOLVE_DEST: return "RESOLVE_DEST";
    case D3D12_RESOURCE_STATE_RESOLVE_SOURCE: return "RESOLVE_SOURCE";
    case D3D12_RESOURCE_STATE_VIDEO_DECODE_READ: return "VIDEO_DECODE_READ";
    case D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE: return "VIDEO_DECODE_WRITE";
    case D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ: return "VIDEO_PROCESS_READ";
    case D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE: return "VIDEO_PROCESS_WRITE";
    case D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ: return "VIDEO_ENCODE_READ";
    case D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE: return "VIDEO_ENCODE_WRITE";
    default: return "UNKNOWN_STATE";
    }
}



LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception)
{
    //時刻を取得して、時刻を名前に入れたファイルを作成。Dumpsディレクトリ以下に出力
    SYSTEMTIME time;
    GetLocalTime(&time);
    wchar_t filePath[MAX_PATH] = { 0 };
    CreateDirectory(L"./Dumps", nullptr);
    StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
    HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    // processId (このexeのId) とクラッシュ (例外)の発生したthreadIdを取得
    DWORD processId = GetCurrentProcessId();
    DWORD threadId = GetCurrentThreadId();
    // 設定情報を入力
    MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
    minidumpInformation.ThreadId = threadId;
    minidumpInformation.ExceptionPointers = exception;
    minidumpInformation.ClientPointers = TRUE;
    // Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
    MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
    // 他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
    return EXCEPTION_EXECUTE_HANDLER;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
    // ID3D12Resourceを格納するポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> pResource = nullptr;
    //ID3D12Resource* pResource = nullptr;

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

    // リソース記述子を作成
    D3D12_RESOURCE_DESC resourceDesc{};
    // バッファリソース。テクスチャの場合はまた別の設定をする
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeInBytes;
    // バッファの場合はこれらは１にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    // バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // リソースを作成
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,        // ヒープのプロパティ
        D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
        &resourceDesc,          // リソースの記述子
        D3D12_RESOURCE_STATE_GENERIC_READ,           // 初期状態
        nullptr,                // Clear値 (バッファの場合はnullptr)
        IID_PPV_ARGS(&pResource) // ID3D12Resourceポインタを取得
    );

    assert(SUCCEEDED(hr));
    pResource->SetName(L"CreateBufferResource()");

    return pResource; // 作成したリソースを返す
};

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
    // ディスクリプタヒープの生成
    ID3D12DescriptorHeap* DescriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    // レンダ―ターゲットビュー用
    DescriptorHeapDesc.Type = heapType;
    // ダブルバッファ用に２つ。多くたってかまわない。
    DescriptorHeapDesc.NumDescriptors = numDescriptors;
    // 
    DescriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    // エラーチェック
    HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));
    // ディスクリプタヒープの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
    return DescriptorHeap;
}

// DXCを使ってShaderをCompileする関数
Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
    // CompileするShaderファイルへのパス
    const std::wstring& filePath,
    // Compilerに仕様するProfile
    const wchar_t* profile,
    // 初期化で生成したものを３つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler)
{
    ///////////////////////////////////////
    //// 1 hlslファイルを読む
    ///////////////////////////////////////
    // これからシェーダーをコンパイルする旨をログに出す
    Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile)));
    // hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    // 読めなかったら停止する
    assert(SUCCEEDED(hr));
    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;

    ///////////////////////////////////////
    //// 2 Compileする
    ///////////////////////////////////////
    LPCWSTR arguments[] = {
        filePath.c_str(),			// コンパイル対象のhlslファイル名
        L"-E", L"main",				// エントリーポイントの指定。基本的にmain
        L"-T", profile,				// ShaderProfileの設定
        L"-Zi", L"-Qembed_debug",	// デバック用の情報を埋め込む
        L"-Od",						// 最適化を外しておく
        L"-Zpr",					// 目盛レイアウトは行優先
    };
    // 実際にシェーダーをコンパイルする
    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,	// 読み込んだファイル
        arguments,				// コンパイルオプション
        _countof(arguments),	// コンパイルオプションの数
        includeHandler,			// includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult)// コンパイル結果
    );
    // コンパイルエラーではなくdxcが起動出来ないなど致命的な状況
    assert(SUCCEEDED(hr));

    ///////////////////////////////////////
    //// 3 警告・エラーが出ていないか確認する
    ///////////////////////////////////////
    // 警告・エラーが出たらログにだして止める
    IDxcBlobUtf8* shaderError = nullptr;
    IDxcBlobUtf16* outputName = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), &outputName);

    if (shaderError != nullptr && shaderError->GetStringLength() != 0)
    {
        Log(shaderError->GetStringPointer());
        assert(false); // コンパイルエラーが発生した場合は停止
    }

    ///////////////////////////////////////
    //// 4 Compile結果を受け取って返す
    ///////////////////////////////////////
    // コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob* shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));
    // 成功したログを出す
    Log(ConvertString(std::format(L"Compile Succeeded. path:{}\n", filePath, profile)));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();
    // 実行用のバイナリを返却
    return shaderBlob;
}


// 2,
Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
    // 1,metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width);
    resourceDesc.Height = UINT(metadata.height);
    resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
    resourceDesc.Format = metadata.format; // TextureのFormat

    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。１固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使ってるのは２次元

    // 2,利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 3,Resourceを生成する
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST, // 初回のResourceState.Textureは基本読むだけ
        nullptr, // Clear最適解。使わないのでnullptr
        IID_PPV_ARGS(&resource) // 作成するResourceポインタへのポインタ
    );
    assert(SUCCEEDED(hr));
    resource->SetName(L"CreateTextureResource()");

    return resource;
}

// 3,TextureResourceにデータを転送する
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
    UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
    // Tetureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READ ResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;
}

// DepthStencilTextureを作る
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
    // 生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.MipLevels = 1; // mipmapの数
    resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // TextureのFormat
    resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。１固定
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // ２次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使うよーとういう通知

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(１番遠い状態)でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceろあわせる

    // Resourceの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&resource)
    );
    assert(SUCCEEDED(hr));

    return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

// mtlファイルを読み込む関数
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
    /////////////////
    // 変数宣言
    /////////////////
    MaterialData materialData;
    std::string line;

    /////////////////
    // ファイルを開く
    /////////////////
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    /////////////////
    // MaterialDataを構築する
    /////////////////
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        // 
        if (identifier == "map_KD")
        {
            std::string textureFilename;
            s >> textureFilename;
            materialData.textureFilePath = directoryPath + "/" + textureFilename;
        }


    }

    /////////////////
    // 構築したMaterialDataをreturnする
    /////////////////
    return materialData;
}

// objファイルを読み込む関数
ModelData LoadOBJFile(const std::string& directoryPath, const std::string& filename)
{
    /////////////////
    // 変数宣言
    /////////////////
    ModelData modelData;
    std::vector<Vector4> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::string line;

    /////////////////
    // ファイルをひらく
    /////////////////
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    /////////////////
    // ModelDataを構築する
    /////////////////
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;
        // 頂点位置
        if (identifier == "v")
        {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.x *= -1.0f;
            position.w = 1.0f;
            positions.push_back(position);
        }
        // 頂点テクスチャ座標
        else if (identifier == "vt")
        {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        }
        // 頂点法線
        else if (identifier == "vn")
        {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normal.x *= -1.0f;
            normals.push_back(normal);
        }
        // 面
        else if (identifier == "f")
        {
            // 1行分の頂点定義をすべて取得
            std::vector<std::string> vertexDefs;
            std::string vertexDefinition;
            while (s >> vertexDefinition)
            {
                vertexDefs.push_back(vertexDefinition);
            }

            // 3頂点未満は無視
            if (vertexDefs.size() < 3) continue;

            // 四角形を三角形２つに五角形を三角形３つに変換
            for (size_t i = 1; i + 1 < vertexDefs.size(); ++i)
            {
                VertexData triangle[3];
                std::string vdefs[3] = { vertexDefs[0], vertexDefs[i], vertexDefs[i + 1] };

                for (int faceVertex = 0; faceVertex < 3; ++faceVertex)
                {
                    std::istringstream v(vdefs[faceVertex]);
                    std::vector<std::string> components;
                    std::string index;
                    while (std::getline(v, index, '/'))
                    {
                        components.push_back(index);
                    }

                    uint32_t posIndex = (components.size() > 0 && !components[0].empty()) ? std::stoi(components[0]) : 0;
                    uint32_t uvIndex = (components.size() > 1 && !components[1].empty()) ? std::stoi(components[1]) : 0;
                    uint32_t normIndex = (components.size() > 2 && !components[2].empty()) ? std::stoi(components[2]) : 0;

                    Vector4 position = { 0,0,0,1 };
                    Vector2 texcoord = { 0,0 };
                    Vector3 normal = { 0,0,0 };

                    if (posIndex > 0 && posIndex <= positions.size())
                        position = positions[posIndex - 1];
                    if (uvIndex > 0 && uvIndex <= texcoords.size())
                        texcoord = texcoords[uvIndex - 1];
                    if (normIndex > 0 && normIndex <= normals.size())
                        normal = normals[normIndex - 1];

                    triangle[faceVertex] = { position, texcoord, normal };
                }

                // 頂点の順序を逆にして追加（右手系→左手系変換のため）
                modelData.vertices.push_back(triangle[2]);
                modelData.vertices.push_back(triangle[1]);
                modelData.vertices.push_back(triangle[0]);
            }
        }

        // mtllib
        else if (identifier == "mtllib")
        {
            // materialTemplateLibraryファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            // 基本的にmtlはobjファイルと同一階層に配置指せるので、ディレクトリ名とファイル名を渡す
            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }
    }

    /////////////////
    // 構築したModelDataをreturnする
    /////////////////
    return modelData;
}