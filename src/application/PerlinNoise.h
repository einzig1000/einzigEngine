#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>
#include "definition/definition.h"



// シンプルな PerlinNoise クラス（perm テーブル方式）
class PerlinNoise
{
public:
    explicit PerlinNoise(uint32_t seed = 0);

    float noise(float x, float y) const;

private:
    std::vector<int> pen;

    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float grad(int hash, float x, float y);
};

struct NoiseParameter
{
	int height = CHUNK_Z;       // マップの高さ
	float scale = 32.0f;        // 地形の粗さ（大きくすると緩やか）
    int octaves = 4;            // 反復回数 (大きくすると細かい起伏が増える)
    float persistence = 0.5f;   // 各オクターブの振幅減衰 (大きくすると細かい起伏が増える)
    uint32_t seed = 12345;      // 俗に言うシード値
	PerlinNoise pn;
};

// フラクタル（オクターブ）合成
float fractalPerlin(const PerlinNoise& pn, float x, float y, int octaves, float persistence);