#include "Game.h"           // クラス定義
#include "functions.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cstdint>


Game::Game(WindowManager& windowManager, DirectXManager& dxManager)
    : windowManager(windowManager), dxManager(dxManager) {

    // 読み込んだオブジェクトの合計
    objectSum = 0;

    // 光源の設定
    directionalLightResource = CreateBufferResource(dxManager.GetDevice(), sizeof(DirectionalLigft));
    directionalLightData = nullptr;
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData->direction = Normalize({ 0.0f, -1.0f, 0.0f });
    directionalLightData->intensity = 1.0f;


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
    objects[obj2].transform.rotate.x += 0.01f;
    objects[obj2].transform.rotate.y += 0.01f;
    objects[obj2].transform.rotate.z += 0.01f;

    // ライトの向きを正規化
    directionalLightData->direction = Normalize(directionalLightData->direction);

    // カメラの設定
    Transforms cameraTransform{ {1.0f,1.0f,1.0f}, {0.3f,0.0f,0.0f}, {0.0f,4.0f,-10.0f} };
    Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
    Matrix4x4 viewMatrix = Inverse(cameraMatrix);
    Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);// int width, int heightをもってくる
    

    for (uint32_t i = 0; i < objects.size(); ++i)
    {
        // オブジェクトのWorldViewProjectionMatrixを作る
        objects[i].transformationMatrixData->World = MakeAffineMatrix(objects[i].transform.scale, objects[i].transform.rotate, objects[i].transform.translate);
        // オブジェクトのWVPMatrixを作る
        objects[i].transformationMatrixData->WVP = Mul(objects[i].transformationMatrixData->World, Mul(viewMatrix, projectionMatrix));
    }

}

void Game::Render() 
{
    dxManager.BeginFrame();

    Drawobj(obj2);
    Drawobj(obj2);



    dxManager.EndFrame();
}

void Game::Drawobj(uint32_t objectNumeber)
{
    // 描画処理
    dxManager.GetCommandList()->IASetVertexBuffers(0, 1, &objects[objectNumeber].vertexBufferView);
    // 形状を設定
    dxManager.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager.GetCommandList()->SetGraphicsRootConstantBufferView(0, objects[objectNumeber].materialResource->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager.GetCommandList()->SetGraphicsRootConstantBufferView(1, objects[objectNumeber].transformationMatrixResource->GetGPUVirtualAddress());
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager.GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager.GetCommandList()->SetGraphicsRootDescriptorTable(2, objects[objectNumeber].textureSrvHandleGPU);

    // 描画
    dxManager.GetCommandList()->DrawInstanced(UINT(objects[objectNumeber].modelData.vertices.size()), 1, 0, 0);
}

int Game::LoadOBJ(const std::string& directoryPath, const std::string& filename)
{
    // ボックスを作成
    Object3D obj;

    // モデルデータ
    //obj.modelData = LoadOBJFile("resources", "axis.obj");
    obj.modelData = LoadOBJFile(directoryPath, filename);

    // 頂点バッファ
    obj.vertexResource = CreateBufferResource(dxManager.GetDevice(), sizeof(VertexData) * obj.modelData.vertices.size());
    obj.vertexBufferView.BufferLocation = obj.vertexResource->GetGPUVirtualAddress();
    obj.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * obj.modelData.vertices.size());
    obj.vertexBufferView.StrideInBytes = sizeof(VertexData);
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
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(obj.textureResource.Get(), mipImage, dxManager.GetDevice(), dxManager.GetCommandList());


    // metaDataを基にSRVの作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    // SRVを作成するDescriptorHeapの場所を決める 先頭はImGuiが使ってるのでその次を使う
    const uint32_t descriptorSizeSRV = dxManager.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(dxManager.GetsrvDescriptorHeap(), descriptorSizeSRV, 1);
    obj.textureSrvHandleGPU = GetGPUDescriptorHandle(dxManager.GetsrvDescriptorHeap(), descriptorSizeSRV, 1);

    // SRVの作成
    dxManager.GetDevice()->CreateShaderResourceView(obj.textureResource.Get(), &srvDesc, textureSrvHandleCPU);

    // 識別ナンバーの設定
    obj.number = objectSum;
    objectSum++;

    objects.push_back(obj);

    return obj.number;
}
