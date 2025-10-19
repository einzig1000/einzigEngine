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
#include "Game.h"
#include <random>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "Dbghelp.lib")


#ifdef _MSC_VER
#define VSNPRINTF_FUNC _vsnprintf_s
#else
#define VSNPRINTF_FUNC vsnprintf
#endif





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


bool IsLooseCollision(const AABB& aabb1, const AABB& aabb2, float threshold)
{
    // 各軸の重なり量（侵入量）を計算
    float dx = my_min(aabb1.max.x, aabb2.max.x) - my_max(aabb1.min.x, aabb2.min.x);
    float dy = my_min(aabb1.max.y, aabb2.max.y) - my_max(aabb1.min.y, aabb2.min.y);
    float dz = my_min(aabb1.max.z, aabb2.max.z) - my_max(aabb1.min.z, aabb2.min.z);

    // どれかの軸で分離している（侵入量が負）なら衝突していない
    if (dx <= 0.0f || dy <= 0.0f || dz <= 0.0f) return false;

    // 侵入量がすべて threshold より大きければ衝突とみなす
    return (dx > threshold && dy > threshold && dz > threshold);
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

        if (IsCollision(ray, t))
        {
            return true; // どれか1つでも当たればtrue
        }
    }
    return false;
}


std::optional<Vector3> IntersectRayTriangle(const Ray& r, const Triangle& t)
{
    // 三角形の法線と平面の距離を求める
    Vector3 edge1 = (t.vertices[1] - t.vertices[0]);
    Vector3 edge2 = (t.vertices[2] - t.vertices[0]);
    Vector3 normal = (edge1.Cross(edge2)).Normalized();
    float distance = (normal.Dot(t.vertices[0]));

    // １，線と三角形の存在する平面の衝突判定
    if (!IsCollision(r, Plane{ normal, distance }))
    {
        return std::nullopt;
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
                return std::nullopt;
            }
        }
    }
    return intersect;
}

std::optional<Vector3> IntersectRayModel(const Ray& ray, const std::vector<VertexData>& vertices, const AABB& aabb, const Transforms& data)
{
    // まずAABBで大まかに判定
    if (!IsCollision(ray, aabb))
    {
        return std::nullopt;
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

        std::optional<Vector3> pos = IntersectRayTriangle(ray, t);

        if (pos != std::nullopt)
        {
            return pos.value(); // どれか1つでも当たればtrue
        }
    }
    return std::nullopt;
}

std::optional<Vector3> IntersectRayAABB(const Ray& ray, const AABB& box)
{
    const float EPSILON = 1e-8f;
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::infinity();

    // スラブ（軸ごとの射線区間）を更新するラムダ
    auto slab = [&](float origin, float dir, float bmin, float bmax) -> bool
        {
            // X方向のベクトルが０に限りなく近ければ、X平面で見た時レイは点のように見える。
            if (std::fabs(dir) < EPSILON)
            {
                // その点がAABBのＸ軸から見た平面内だったら衝突しているといえる。Ｙ軸Ｚ軸についても同様のことであってくれ
                return (origin >= bmin && origin <= bmax);
            }


            float invD = 1.0f / dir;
            float t1 = (bmin - origin) * invD;
            float t2 = (bmax - origin) * invD;
            // 
            if (t1 > t2) std::swap(t1, t2);
            tmin = my_max(tmin, t1);
            tmax = my_min(tmax, t2);
            return (tmax >= tmin);
        };

    if (!slab(ray.origin.x, ray.diff.x, box.min.x, box.max.x)) return std::nullopt;
    if (!slab(ray.origin.y, ray.diff.y, box.min.y, box.max.y)) return std::nullopt;
    if (!slab(ray.origin.z, ray.diff.z, box.min.z, box.max.z)) return std::nullopt;

    // レイがボックスの後方にしかない場合は衝突なし
    if (tmax < 0.0f) return std::nullopt;

    // tmin が正なら最初の交差、負ならボックス内スタート → tmax を使う
    float tHit = (tmin >= 0.0f) ? tmin : tmax;
    return ray.origin + ray.diff * tHit;
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

#pragma region Rand

int RandomInt(int min, int max)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

float RandomFloat(float min, float max, int decimalPlaces)
{
    int scale = static_cast<int>(std::pow(10, decimalPlaces));
    int intMin = static_cast<int>(std::round(min * scale));
    int intMax = static_cast<int>(std::round(max * scale));
    int randomInt = RandomInt(intMin, intMax);
    return static_cast<float>(randomInt) / scale;
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


float ToRadian(const float& angle)
{
    return angle * (std::numbers::pi_v<float> / 180.0f);
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
    const float kLonEvery = float((2 * std::numbers::pi_v<float>) / kSubdivision);
    // 緯度分割１つ分の角度
    const float kLatEvery = float(std::numbers::pi_v<float> / kSubdivision);

    // 緯度の方向に分割 -π/2 ～ π/2
    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
    {
        // 現在の緯度と次の緯度
        const float lat = float(-std::numbers::pi_v<float> / 2.0f + latIndex * kLatEvery);
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

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
    ID3D12Device* device, size_t sizeInBytes)
{
    // ID3D12Resourceを格納するポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> pResource = nullptr;

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

Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantBufferResource(
    ID3D12Device* device, size_t sizeInBytes)
{
    size_t ConstantSize;
    ConstantSize =  (sizeInBytes + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

    return CreateBufferResource(device, ConstantSize);
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
