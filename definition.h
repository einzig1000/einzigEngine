#pragma once

struct Vector2
{
    float x = 0;
    float y = 0;
};

struct Vector3
{
    float x = 0;
    float y = 0;
    float z = 0;
};

struct Vector4 {
    float x, y, z, w;
};

struct Matrix3x3
{
    float m[3][3] = {
    { 0,0,0 },
    { 0,0,0 },
    { 0,0,0 } };
};

struct Matrix4x4
{
    float m[4][4] = {
    { 0,0,0,0 },
    { 0,0,0,0 },
    { 0,0,0,0 },
    { 0,0,0,0 } };
};

struct Vertex
{
    Vector2 LT;
    Vector2 RT;
    Vector2 LB;
    Vector2 RB;
};

struct Transforms
{
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};

struct VertexData
{
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};

struct Material
{
    Vector4 color;
    int32_t enableLighting;
};

struct TransformationMatrix
{
    Matrix4x4 WVP;
    Matrix4x4 World;
};

struct DirectionalLigft
{
    Vector4 color;
    Vector3 direction;
    float intensity;//輝度
};