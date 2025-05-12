#include "Game.h"           // クラス定義
#include "functions.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cstdint>


Game::Game(WindowManager& windowManager, DirectXManager& dxManager)
    : windowManager(windowManager), dxManager(dxManager) {
}

void Game::Run()
{
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
            Render();
        }
    }
}

void Game::Update()
{
    // ゲームロジックの更新


}

void Game::Render() 
{
    dxManager.BeginFrame();

    // 描画処理




    dxManager.EndFrame();
}

void Game::LoadOBJ(const std::string& directoryPath, const std::string& filename)
{
    // ボックスを作成
    Object3D obj;

    // モデルデータ
    obj.modelData = LoadOBJFile("resources", "axis.obj");

    // 頂点バッファ
    obj.vertexResource = CreateBufferResource(dxManager.GetDevice(), sizeof(VertexData) * obj.modelData.vertices.size());
    obj.vertexBufferView.BufferLocation = obj.vertexResource->GetGPUVirtualAddress();
    VertexData* vertexData = nullptr;
    obj.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    std::memcpy(vertexData, obj.modelData.vertices.data(), sizeof(VertexData) * obj.modelData.vertices.size());

    // マテリアルデータ
    obj.materialResource = CreateBufferResource(dxManager.GetDevice(), sizeof(Material));
    obj.materialData = nullptr;
    obj.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&obj.materialData));
    obj.materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    obj.materialData->enableLighting = true;
    obj.materialData->uvTransform = MakeIdentity4x4();

    // 変換行列
    obj.transform = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

    // ワールド・ビュー・プロジェクション行列
    obj.transformationMatrixResource = CreateBufferResource(dxManager.GetDevice(), sizeof(TransformationMatrix));
    obj.transformationMatrixData = nullptr;
    obj.transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&obj.transformationMatrixData));
    obj.transformationMatrixData->World = MakeIdentity4x4();
    obj.transformationMatrixData->WVP = MakeIdentity4x4();
    obj.transformationMatrixResource->Unmap(0, nullptr);


    // テクスチャ
    DirectX::ScratchImage mipImage = LoadTexture("resources/uvChecker.png");
    const DirectX::TexMetadata& metadata = mipImage.GetMetadata();
    obj.textureResource = CreateTextureResource(dxManager.GetDevice(), metadata);
    //Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(obj.textureResource.Get(), mipImage, dxManager.GetDevice(), commandList.Get());
    //Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResource(dxManager.GetDevice(), kClientWidth, kClientHeight);


    objects.push_back(obj);

}
