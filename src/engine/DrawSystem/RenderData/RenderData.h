#pragma once
#include "definition/definition.h"
#include <optional>

class Block;

class RenderData_Model
{
public:
    RenderData_Model();
    ~RenderData_Model();
    // 全オブジェクトのSRT更新,それに伴うワールド行列更新
    void Update1();
    // 全オブジェクトの親を考慮したワールド行列更新,AABB更新
    void Update2();
    // 衝突判定,衝突ペア・深度の保存
    void Update3();
    // 衝突時の更新,それに伴うワールド行列更新
    void Update4();
    // 全オブジェクトの描画範囲内判定,前フレーム情報保存
    void Update5();

    std::optional<std::string> name;
    VectorDynamics scale = { Vector3(1.0f,1.0f,1.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f) };
    VectorDynamics rotate = { Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f) };
    VectorDynamics translate = { Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f), Vector3(0.0f,0.0f,0.0f) };
    // 親のワールドマトリックス
    RenderData_Model* parentModel = nullptr;
    // 今フレームの移動量
    Vector3 lastMove;
    // UV座標
    Transforms uvTransform;
    // 色
    Vector4 color = { 0xFF, 0xFF, 0xFF, 0xFF };
    // 3Dモデル
    int32_t model = -1;
    // テクスチャ
    int32_t texture = -1;
	int32_t additionalTexture = -1;
    // 描画オプション
    DrawOptions options;
    // 衝突判定用AABB
    std::vector<AABB> aabbs;
    // 重さ
    float mass = 1.0f;
    // ファイルパス
    std::string filePath = "resources/Prototypes/model_json/aaa";
    // 画面内に存在するか
    bool inPicture = false;
    // マウスとの衝突判定
    int isCollisionMouseRay = -1; // -1:非衝突, 0:最初に衝突, 1:2番目


    // 他のオブジェクトとの衝突判定
    bool isCollision(RenderData_Model* target) const;
    // 衝突時すりぬけないオブジェクトの設定
    void SetBlock(RenderData_Model* target);

    // 任意のポイントを向く
    void LookAtOnce(const Vector3& targetWorldPos, float roll = 0);
    void LookAtOnce(const RenderData_Model* other, float roll = 0);
    void LookAtCamera(float roll = 0);
    void LookAtFront(float roll = 0);

    // ワールド位置を返す
    Matrix4x4 GetWorldMatrix() const;
    Vector3 GetWorldPosition() const;

    // 描画
    void Draw();
    void DrawAABB();
    void DrawImGui();

    
    // マウスと衝突判定とるか否か
	bool isCheckMouseRay = false;

    static std::vector<RenderData_Model*> renderModels;

private:
    // 今フレームでS/R/Tに変化があったか
    bool movedThisFrame = true;

    // 衝突ペアを全部保存
    static std::vector<CollisionInf*> collisionInfos;

    // 親を考慮しないワールドマトリックス
    Matrix4x4 localWorldMatrix;
    // 親を考慮したワールドマトリックス
    Matrix4x4 worldMatrix;
    Vector3 worldPos;

    // 親を考慮したワールドマトリックスを設定する
    Matrix4x4 SetWorldMatrix();

    // ID
    int ID = 0;


    // 初期化済みフラグ
    bool initialized = false;

    // 衝突フラグ
    unsigned int CollisionFlags = 0x00000000;
    // 前フレームの衝突フラグ
    unsigned int preCollisionFlags = 0x00000000;
    // 衝突したときの反発係数
    const float restitution = 0.0f;

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

class RenderData_Rect
{
public:
    RenderData_Rect();
    ~RenderData_Rect();

    // 位置、回転、スケール
    Transforms transforms;
    // UV座標
    Transforms uvTransform;

    // 右上
    Vector3 pos1;
    // 右下
    Vector3 pos2;
    // 左上
    Vector3 pos3;
    // 左下
    Vector3 pos4;

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

    static std::vector<RenderData_Rect*> renderRects;
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
    static void UpdateAllParticles();
    void Update();

    // ID
    std::optional<std::string> name;

    // ファイルパス
    std::string filePath = "resources/Prototypes/particle/aaa";

    bool LoadJson();

    void Draw();
    void DrawImGui();
    void DrawEmitter();


    /// リソース
    uint32_t model = 0;
    uint32_t texture = 0;

    /// エミッター
    PrimitiveType emitterShape = PrimitiveType::AABB;
    Sphere emitterSphere = { Vector3{0.0f,0.0f,0.0f}, 1.0f };
    SphereXYZ emitterSphereXYZ = { Vector3{0.0f,0.0f,0.0f}, Vector3{1.0f,1.0f,1.0f} };
    AABB emitterAABB = { Vector3{ -10.0f, -10.0f, -10.0f }, Vector3{ 10.0f, 10.0f, 10.0f } };
    bool emitFromInside = true;     // 内側から出るか外殻上から出るか

    /// 密度
    int32_t particlesPerEmission = 1;   // 1フレで生む数
    int32_t emissionDelay = 1;          // 生成間隔フレーム
    int32_t liveMax = 300;              // 寿命フレーム(マイナスの時は不老)

    /// 方向
    bool useTarget = false;             // ターゲット方向に飛ばすかどうか
    bool spawnDependent = false;        // 発生位置に依存した方向に飛ばすかどうか
    Vector3 target;
    float speed = 1.0f;                 // 速度
    float spreadAngle = 0.0f;           // 拡散角度

    /// オプション
    bool isBillboard = true;    // ビルボードかどうか
	BlendMode blendMode = BlendMode::kBlendModeAdd;

    /// SRT
    ParticleSRT targetScale;
    ParticleSRT targetRotate;
    ParticleSRT targetTranslate;

    /// マテリアル
    uint32_t color = 0xFFFFFFFF;
    Matrix4x4 uvTransform;


    uint32_t GetCurrentSum() const { return currentSum; }



	uint32_t capacity = 1024;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
    Matrix4x4* instancingData_ = nullptr;
	SRVAllocation srvAllocation_;

	std::vector<VectorDynamics> scale_;
	std::vector<VectorDynamics> rotate_;
	std::vector<VectorDynamics> translate_;
    std::vector<uint32_t> lifeCount_;
	std::vector<bool> isActive_;

private:
    /// 現在存在するパーティクル数
    uint32_t currentSum = 0;

    int ID = 0;

    uint32_t frame = 0;                 // 経過フレーム

	//// パーティクル生成
    void SpawnParticle();
    // 生まれる場所設定
	void SetSpawnPosition(uint32_t index);
	// SRTの設定
	void SetSpawnScale(uint32_t index);
	void SetSpawnRotate(uint32_t index);
	void SetSpawnTranslate(uint32_t index);

    //// ワールド行列・WVP行列の更新
	void UpdateTransformationMatrix();
	//// 各パーティクルの変換行列更新
	void UpdateTransforms();

	//// 寿命管理
	void UpdateLife();

	//// 死亡判定
	void CheckLife();

	//// 非アクティブかしたパーティクルの削除
	void RemoveInactiveParticles();

    // ロードした結果
    bool loadResult = false;

    static std::vector<RenderData_Particle*> renderParticles;
};

class RenderData_Block
{
public:

    // 新しいブロックを作るときはAddNewBlock()
	// ブロックを壊すときはRemoveBlockFromList()

    RenderData_Block(BlockID id);
    ~RenderData_Block();
    static void UpdateAllBlock();
    void Update();

	//// リストに新たなブロックを追加
	void AddNewBlock(Vector3 position, Vector3int index);
	//// リストからブロックを削除
	void RemoveBlock(Vector3int index);

    // 非アクティブなブロックの削除
	void RemoveInactiveBlocks();

    // ID
    BlockID name;

    void Draw();
    void DrawImGui();


    /// リソース
    uint32_t model = 0;
    uint32_t texture = 0;
	uint32_t additionalTexture = 0;

    /// オプション
    BlendMode blendMode = BlendMode::kBlendModeAdd;

    /// マテリアル
    uint32_t color = 0xFFFFFFFF;
    Matrix4x4 uvTransform;

    /// 現在存在するブロック数
    uint32_t currentSum = 0;

    uint32_t capacity = 4096;
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
    Matrix4x4* instancingData_ = nullptr;
    SRVAllocation srvAllocation_;

    std::vector<VectorDynamics> scale_;
    std::vector<VectorDynamics> rotate_;
    std::vector<VectorDynamics> translate_;
	std::vector<Vector3int> indexes_;
	std::vector<uint32_t> colors_;
    std::vector<bool> isActive_;

private:

    int ID = 0;


    //// ワールド行列・WVP行列の更新
    void UpdateWorldMatrix();
    //// 各パーティクルの変換行列更新
    void UpdateTransforms();

    //// 死亡判定
    void CheckLife();

    // ロードした結果
    bool loadResult = false;

	static std::vector<RenderData_Block*> renderBlocks;
};