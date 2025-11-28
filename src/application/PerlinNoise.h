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

    double noise(double x, double y) const;

private:
    std::vector<int> pen;

    static double fade(double t);
    static double lerp(double a, double b, double t);
    static double grad(int hash, double x, double y);
};

// フラクタル（オクターブ）合成
double fractalPerlin(const PerlinNoise& pn, double x, double y, int octaves, double persistence);