#pragma once
#include "definition/definition.h"
#include <optional>

class RenderData_Model
{
public:
    RenderData_Model();
    ~RenderData_Model();
    void Update();

    VectorDynamics scale = { Vector3( 1.0f,1.0f,1.0f ), Vector3( 0.0f,0.0f,0.0f ), Vector3( 0.0f,0.0f,0.0f ) };
	VectorDynamics rotate = { Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f) };
	VectorDynamics translate = { Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f) };
	// 親のワールドマトリックス
	Matrix4x4* parentMatrix = nullptr;
    // 今フレームの移動量
    Vector3 lastMove;
    // 今フレームでS/R/Tに変化があったか
    bool movedThisFrame = true;
    // 回転の中心点
    Vector3 pivot;
    // UV座標
    Transforms uvTransform;
    // 色
    uint32_t color = 0xFFFFFFFF;
    // 3Dモデル
    uint32_t model = 0;
    // テクスチャ
    uint32_t texture = 0;
    // 描画オプション
    DrawOptions options;
    // 衝突判定用AABB
    std::vector<AABB> aabbs;
    // 重さ
    float mass = 1.0f;
    // ID
    int ID = 0;
    std::optional<std::string> name;
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
    TransformationMatrix transformationMatrix;
    bool initialized = false;

    // 衝突フラグ
    unsigned int CollisionFlags = 0x00000000;
    // 前フレームの衝突フラグ
    unsigned int preCollisionFlags = 0x00000000;
    // 衝突したときの反発係数
    const float restitution = 0.0f;

    // 衝突方向に応じた行動
    void CollisionAction(const Vector3& depth, RenderData_Model& target);

    // ワールドマトリックスの更新
	void UpdateWorldMatrix();

    Vector3 worldPos;
    // 前フレーム位置、回転、スケール、ワールド座標
    VectorDynamics preScale;
    VectorDynamics preRotate;
    VectorDynamics preTranslate;
	Vector3 preWorldPos;
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
    Anchor anchor = Anchor::Center;
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
    // 画像切り取り左上
	Vector2int cutImageLeftTop = { 0,0 };
    // 切り出しサイズ
	Vector2int cutImageSize = { 0,0 };
  
    // マウスと衝突してるか？
    bool isCollisionMouseRay = false;
    // ID
    int ID = 0;
    std::optional<std::string> name;

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
    std::optional<std::string> name;

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
    std::optional<std::string> name;
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
    std::optional<std::string> name;

    // ファイルパス
    std::string filePath = "resources/Prototypes/particle/aaa";

    bool LoadJson();

    void Draw();
    void DrawImGui();
    void DrawEmitter();

    ParticleInf& GetParticleInf() { return particleInf; }
    /// 現在存在するパーティクル数
    uint32_t currentSum = 0;

private:
    ParticleInf particleInf;



    int ID = 0;

    // ロードした結果
    bool loadResult = false;

    static std::vector<RenderData_Particle*> renderParticles;
};