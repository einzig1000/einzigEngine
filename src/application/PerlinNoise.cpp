#include "PerlinNoise.h"

PerlinNoise::PerlinNoise(unsigned int seed)
{
    pen.resize(256);
    std::iota(pen.begin(), pen.end(), 0);
    std::mt19937 gen(seed);
    std::shuffle(pen.begin(), pen.end(), gen);
    // 512長に拡張
    pen.insert(pen.end(), pen.begin(), pen.end());
}

double PerlinNoise::noise(double x, double y) const
{
    // 単純な実装: 周期なし
    int xi = static_cast<int>(std::floor(x)) & 255;
    int yi = static_cast<int>(std::floor(y)) & 255;
    double xf = x - std::floor(x);
    double yf = y - std::floor(y);

    double u = fade(xf);
    double v = fade(yf);

    int aa = pen[pen[xi] + yi];
    int ab = pen[pen[xi] + yi + 1];
    int ba = pen[pen[xi + 1] + yi];
    int bb = pen[pen[xi + 1] + yi + 1];

    double x1 = lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u);
    double x2 = lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u);
    double res = lerp(x1, x2, v);

    // Perlin の出力は概ね -1..1 なので 0..1 に正規化
    return (res + 1.0) / 2.0;
}

double PerlinNoise::fade(double t)
{
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double a, double b, double t)
{
    return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y)
{
    // hash の下位ビットで勾配ベクトルを選ぶ（簡易）
    switch (hash & 3)
    {
    case 0: return  x + y;
    case 1: return -x + y;
    case 2: return  x - y;
    case 3: return -x - y;
    default: return 0;
    }
}

double fractalPerlin(const PerlinNoise& pn, double x, double y, int octaves, double persistence)
{

    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;
    for (int i = 0; i < octaves; ++i)
    {
        total += pn.noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }
    return total / maxValue; // 0..1 に近い値
}