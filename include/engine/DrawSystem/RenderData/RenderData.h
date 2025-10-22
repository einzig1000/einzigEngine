#pragma once
#include "definition/definition.h"
#include <optional>

class RenderData_Model
{
public:
    RenderData_Model();
    ~RenderData_Model();
    void Update(std::vector<Object3D>& objects);

    // 今フレーム位置、回転、スケール
    Transforms transforms;
    // 今フレームの移動量
    Vector3 lastMove;
    // 今フレームでS/R/Tに変化があったか
    bool movedThisFrame = true;
    // 回転の中心点
    Vector3 pivot;
    // UV座標
    Transforms uvTransform;
    // 速度
    Vector3 velocity;
    // 加速度
    Vector3 acceleration;
    // 重力加速度
    Vector3 gravity;
    // 色
    uint32_t color = 0xFFFFFFFF;
    // 3Dモデル
    uint32_t model = 0;
    // テクスチャ
    uint32_t texture = 0;
    // 描画オプション
    DrawOptions options;
    // 衝突判定用AABB
    std::vector<AABB> aabb;
    // 重さ
    float mass = 1.0f;
    // ID
    int ID = 0;
    std::string name = "NULL";
    // 画面内に存在するか
    bool inPicture = false;
    // マウスとの衝突判定
    int isCollisionMouseRay = -1; // -1:非衝突, 0:最初に衝突, 1:2番目


    // 他のオブジェクトとの衝突判定
    bool isCollision(RenderData_Model& target) const;
    // 衝突時すりぬけないオブジェクトの設定
    void SetBlock(RenderData_Model& target);

    // 任意のポイントを向く
    void LookAtOnce(const Vector3& targetWorldPos, float roll = 0);
    void LookAtOnce(const RenderData_Model& other, float roll = 0);
    void LookAtCamera(float roll = 0);
    void LookAtFront(float roll = 0);

    // ワールド位置を返す
    Matrix4x4 GetWorldMatrix() const;
    Vector3 GetWorldPosition() const;

    // 描画
    void Draw();
    void DrawAABB();
    void DrawImGui();


    static std::vector<RenderData_Model*> renderModels;

private:

    // 衝突フラグ
    unsigned int CollisionFlags = 0x00000000;
    // 前フレームの衝突フラグ
    unsigned int preCollisionFlags = 0x00000000;
    // 衝突したときの反発係数
    //const float restitution = 0.0f;
    // 衝突方向に応じた行動
    void CollisionAction(const Vector3& depth, RenderData_Model& target);


    // 前フレーム位置、回転、スケール
    Transforms preTransforms;
    // 前フレームAABB
    std::vector<AABB> preAABB;

    // 他のオブジェクトと衝突したときのAABBのインデックスペア
    std::optional<CollisionInf> isCollisionAABBInf(RenderData_Model& target) const;

    std::vector<RenderData_Model*> blockList;
};

class RenderData_Sprite
{
public:
    RenderData_Sprite();
    ~RenderData_Sprite();

    // 位置、回転、スケール
    Transforms transforms;
    // UV座標
    Transforms uvTransform;
    // アンカー
    Anker anker = Anker::Center;
    // 親のワールドマトリックス
    TransformationMatrix parentTransformationMatrix;
    // 回転の中心点
    Vector2 pivot = { 0,0 };
    // 色
    uint32_t color = 0xFFFFFFFF;
    // テクスチャ
    uint32_t texture = 0;
    // 描画オプション
    DrawOptions options;
    // マウスと衝突してるか？
    bool isCollisionMouseRay = false;
    // ID
    int ID = 0;
    std::string name = "NULL";

    void Draw();
    void DrawImGui();

private:

    static std::vector<RenderData_Sprite*> renderSprites;

};

class RenderData_Triangle
{
public:
    RenderData_Triangle();
    ~RenderData_Triangle();

    // 位置、回転、スケール
    Transforms transforms;
    // UV座標
    Transforms uvTransform;
    // 上
    Vector3 pos1 = { 0.0f,  0.5f, 0.0f };
    // 右下
    Vector3 pos2 = { 0.5f, -0.5f, 0.0f };
    // 左下
    Vector3 pos3 = { -0.5f, -0.5f, 0.0f };
    // テクスチャ
    uint32_t texture = 0;
    // 色
    uint32_t color = 0xFFFFFFFF;
    // 描画オプション
    DrawOptions options;
    // ID
    int ID = 0;
    std::string name = "NULL";

    void Draw();
    void DrawImGui();

private:

    static std::vector<RenderData_Triangle*> renderTriangles;

};

class RenderData_Line
{
public:

    RenderData_Line();
    ~RenderData_Line();

    // 点([0]は使用負荷)
    std::vector<Vector3> points;
    // ID
    int ID = 0;
    std::string name = "NULL";
    // 色
    uint32_t color = 0xFFFFFFFF;
    // ラインタイプ
    LineType lineType = LineType::Line;
	// 補完分割数(線形補完時のみ有効)
	uint32_t kSubdivision = 10;

    void Draw();
	void DrawPoints();
	void DrawImGui();

private:

    static std::vector<RenderData_Line*> renderLines;
};

class RenderData_Particle
{
public:

    RenderData_Particle();
    ~RenderData_Particle();

    // ID
    int ID = 0;
    std::string name = "NULL";

    /// エミッター範囲
    AABB emitterAABB = { Vector3{ -1.0f, -1.0f, -1.0f }, Vector3{ 1.0f, 1.0f, 1.0f } };
    SphereXYZ emitterSphere;
    /// エミッター範囲中心から見た時の飛んでく方向
    Vector3 target;

    /// パーティクル１粒
    RenderData_Model mono;

    particleSRT scale = particleSRT{ Vector3{0.5f,0.5f,0.1f},Vector3{-0.01f,-0.01f,-0.01f},Vector3{0.0f,0.0f,0.0f} };
    particleSRT rotate = particleSRT{ Vector3{0.3f,0.3f,0.3f},Vector3{0.0f,0.0f,0.0f},Vector3{0.0f,0.0f,0.0f} };
    particleSRT translate = particleSRT{ Vector3{0.0f,0.0f,0.0f},Vector3{0.0f,-0.1f,0.0f},Vector3{0.0f,0.0f,0.0f} };



    /// ビルボードか
	bool isBillboard = true;

    /// 現在存在するパーティクル数
    uint32_t currentSum = 0;

    /// フレーム
	uint32_t frame = 0;

    /// 生まれる周期
	uint32_t emissionDelay = 1;

    /// １フレームで生まれる量
    uint32_t particlesPerEmission = 1;

    /// 寿命(マイナスの時は不老)
	int liveMax = 300;

    void Draw();
    void DrawImGui();
    void DrawEmitter();

private:

    static std::vector<RenderData_Particle*> renderParticles;
};