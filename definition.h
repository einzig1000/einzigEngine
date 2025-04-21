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
    Vector2 LT_ = { 0,0 };
    Vector2 RT_ = { 0,0 };
    Vector2 LB_ = { 0,0 };
    Vector2 RB_ = { 0,0 };
};
