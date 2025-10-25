#include "Resource/Model/ModelManager.h"
#include "Utilities/functions.h"
#include <filesystem> 
#include <fstream>


ModelManager::ModelManager()
{}

ModelManager::~ModelManager()
{}


uint32_t ModelManager::LoadModel(const std::string& directoryPath, const std::string& filename, ID3D12Device* device)
{
    auto path = directoryPath + "/" + filename;

    auto exists = std::find_if(
        objects.begin(), objects.end(),
        [&path](const Object3D& model) { return model.filePath == path; }
    );
    if (exists != objects.end())
    {
        return exists->number;
    }

    // ボックスを作成
    Object3D obj;
    // モデルデータ
    obj.modelData = LoadModelFile(directoryPath, filename);
    // 変換行列
    obj.transform = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
    // AABB .obj → .csv へ拡張子を変換して渡す
    std::string csvFilename = filename;
    size_t dotPos = csvFilename.rfind('.');
    if (dotPos != std::string::npos)
        csvFilename.replace(dotPos, csvFilename.length() - dotPos, ".csv");
    else
        csvFilename += ".csv";
    csvFilename = directoryPath + csvFilename;
    obj.aabb = LoadAABB(csvFilename, obj.modelData);
    // 識別ナンバー
    obj.number = static_cast<uint32_t>(objects.size());
    // ファイルパス
    obj.filePath = path;

    // まず空のObject3Dをvectorに追加し、参照を取得
    objects.push_back(obj);
    Object3D& ref = objects.back();

    // 頂点バッファ作成
    ref.vertexBufferSize = sizeof(VertexData) * UINT(ref.modelData.vertices.size());
    ref.vertexBuffer = CreateBufferResource(device, ref.vertexBufferSize);
    VertexData* vData = nullptr;
    ref.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    std::memcpy(vData, ref.modelData.vertices.data(), ref.vertexBufferSize);
    ref.vertexBuffer->Unmap(0, nullptr);

    ref.vertexBufferView.BufferLocation = ref.vertexBuffer->GetGPUVirtualAddress();
    ref.vertexBufferView.SizeInBytes = static_cast<UINT>(ref.vertexBufferSize);
    ref.vertexBufferView.StrideInBytes = sizeof(VertexData);

    return ref.number;
}

Object3D* ModelManager::GetModel(uint32_t modelID)
{
	if (modelID < objects.size())
	{
		return &objects[modelID];
	}
	else
	{
		Log("存在しないモデルIDです:%d", modelID);
		return nullptr;
	}
}



// AABB.csvの読み込み & 存在しなければ作成,保存
std::vector<AABB> ModelManager::LoadAABB(const std::string& csvPath, const ModelData& model)
{
	std::vector<AABB> aabbs;
	if (std::filesystem::exists(csvPath))
	{
		aabbs = LoadAABBFromCSV(csvPath);
	}
	else
	{
		// 今までの方法でAABBを1つ作成
		AABB aabb = CreateLocalAABB(model);
		aabbs.push_back(aabb);
		SaveAABBToCSV(csvPath, aabbs);
	}
	return aabbs;
}

// AABB.csvの読み込み
std::vector<AABB> ModelManager::LoadAABBFromCSV(const std::string& csvPath)
{
    std::vector<AABB> aabbs;
    std::ifstream file(csvPath);
    if (!file.is_open()) return aabbs;

    std::string line;
    // 1行目はヘッダーなのでスキップ
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string token;
        std::vector<float> values;
        while (std::getline(ss, token, ','))
        {
            values.push_back(std::stof(token));
        }
        if (values.size() == 6)
        {
            AABB aabb;
            aabb.min = { values[0], values[1], values[2] };
            aabb.max = { values[3], values[4], values[5] };
            aabbs.push_back(aabb);
        }
    }
    return aabbs;
}

// AABBの作成
AABB ModelManager::CreateLocalAABB(const ModelData& model)
{
	AABB localAABB;

	// 最小値と最大値を初期化
	localAABB.min.x = (std::numeric_limits<float>::max)();
	localAABB.min.y = (std::numeric_limits<float>::max)();
	localAABB.min.z = (std::numeric_limits<float>::max)();

	localAABB.max.x = std::numeric_limits<float>::lowest();

	// 頂点データ空だったらエラー出すべきだけどunityシステムあるかもだから落とさない
	if (model.vertices.empty())
	{
		localAABB.min = { 0.0f, 0.0f, 0.0f };
		localAABB.max = { 0.0f, 0.0f, 0.0f };
		return localAABB;
	}

	// 全ての頂点を調べてAABBの最小値と最大値を更新
	for (const auto& vertex : model.vertices)
	{
		// 各軸の最小値を更新
		if (vertex.position.x < localAABB.min.x) localAABB.min.x = vertex.position.x;
		if (vertex.position.y < localAABB.min.y) localAABB.min.y = vertex.position.y;
		if (vertex.position.z < localAABB.min.z) localAABB.min.z = vertex.position.z;

		// 各軸の最大値を更新
		if (vertex.position.x > localAABB.max.x) localAABB.max.x = vertex.position.x;
		if (vertex.position.y > localAABB.max.y) localAABB.max.y = vertex.position.y;
		if (vertex.position.z > localAABB.max.z) localAABB.max.z = vertex.position.z;
	}

	return localAABB;
}

// AABB.csvの作成、保存
void ModelManager::SaveAABBToCSV(const std::string& csvPath, const std::vector<AABB>& aabbs)
{
    std::ofstream file(csvPath);
    file << "min_x,min_y,min_z,max_x,max_y,max_z\n";
    for (const auto& aabb : aabbs)
    {
        file << aabb.min.x << "," << aabb.min.y << "," << aabb.min.z << ","
            << aabb.max.x << "," << aabb.max.y << "," << aabb.max.z << "\n";
    }
}


// mtlファイルを読み込む関数
std::string ModelManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
    /////////////////
    // 変数宣言
    /////////////////
    std::string filePath;
    std::string line;

    /////////////////
    // ファイルを開く
    /////////////////
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    /////////////////
    // MaterialDataを構築する
    /////////////////
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        // 
        if (identifier == "map_KD")
        {
            std::string textureFilename;
            s >> textureFilename;
            filePath = directoryPath + "/" + textureFilename;
        }
    }

    /////////////////
    // 構築したMaterialDataをreturnする
    /////////////////
    return filePath;
}

// objファイルを読み込む関数
ModelData ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
    /////////////////
    // 変数宣言
    /////////////////
    ModelData modelData;
    std::vector<Vector4> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::string line;

    /////////////////
    // ファイルをひらく
    /////////////////
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    /////////////////
    // ModelDataを構築する
    /////////////////
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;
        // 頂点位置
        if (identifier == "v")
        {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.x *= -1.0f;
            position.w = 1.0f;
            positions.push_back(position);
        }
        // 頂点テクスチャ座標
        else if (identifier == "vt")
        {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        }
        // 頂点法線
        else if (identifier == "vn")
        {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normal.x *= -1.0f;
            normals.push_back(normal);
        }
        // 面
        else if (identifier == "f")
        {
            // 1行分の頂点定義をすべて取得
            std::vector<std::string> vertexDefs;
            std::string vertexDefinition;
            while (s >> vertexDefinition)
            {
                vertexDefs.push_back(vertexDefinition);
            }

            // 3頂点未満は無視
            if (vertexDefs.size() < 3) continue;

            // 四角形を三角形２つに五角形を三角形３つに変換
            for (size_t i = 1; i + 1 < vertexDefs.size(); ++i)
            {
                VertexData triangle[3];
                std::string vdefs[3] = { vertexDefs[0], vertexDefs[i], vertexDefs[i + 1] };

                for (int faceVertex = 0; faceVertex < 3; ++faceVertex)
                {
                    std::istringstream v(vdefs[faceVertex]);
                    std::vector<std::string> components;
                    std::string index;
                    while (std::getline(v, index, '/'))
                    {
                        components.push_back(index);
                    }

                    uint32_t posIndex = (components.size() > 0 && !components[0].empty()) ? std::stoi(components[0]) : 0;
                    uint32_t uvIndex = (components.size() > 1 && !components[1].empty()) ? std::stoi(components[1]) : 0;
                    uint32_t normIndex = (components.size() > 2 && !components[2].empty()) ? std::stoi(components[2]) : 0;

                    Vector4 position = { 0,0,0,1 };
                    Vector2 texcoord = { 0,0 };
                    Vector3 normal = { 0,0,0 };

                    if (posIndex > 0 && posIndex <= positions.size())
                        position = positions[posIndex - 1];
                    if (uvIndex > 0 && uvIndex <= texcoords.size())
                        texcoord = texcoords[uvIndex - 1];
                    if (normIndex > 0 && normIndex <= normals.size())
                        normal = normals[normIndex - 1];

                    triangle[faceVertex] = { position, texcoord, normal };
                }

                // 頂点の順序を逆にして追加（右手系→左手系変換のため）
                modelData.vertices.push_back(triangle[2]);
                modelData.vertices.push_back(triangle[1]);
                modelData.vertices.push_back(triangle[0]);
            }
        }

        // mtllib
        else if (identifier == "mtllib")
        {
            // materialTemplateLibraryファイルの名前を取得する
            std::string materialFilename;
            s >> materialFilename;
            // 基本的にmtlはobjファイルと同一階層に配置指せるので、ディレクトリ名とファイル名を渡す
            //modelData = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }
    }

    /////////////////
    // 構築したModelDataをreturnする
    /////////////////
    return modelData;
}
