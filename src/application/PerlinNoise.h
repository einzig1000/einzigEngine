#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>

// シンプルな PerlinNoise クラス（perm テーブル方式）
class PerlinNoise
{
public:
    explicit PerlinNoise(unsigned int seed = 0);

    float noise(float x, float y) const;

private:
    std::vector<int> pen;

    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float grad(int hash, float x, float y);
};

// フラクタル（オクターブ）合成
float fractalPerlin(const PerlinNoise& pn, float x, float y, int octaves, float persistence);