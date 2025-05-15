#include "Game.h"           // クラス定義
#include "functions.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cstdint>


Game::Game(WindowManager& windowManager, DirectXManager& dxManager) : windowManager(windowManager), dxManager(dxManager)
{
    /// imguiの初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(windowManager.GetHwnd());
    ImGui_ImplDX12_Init(
        dxManager.GetDevice(),
        dxManager.GetSwapChainDesc().BufferCount,
        dxManager.GetRtvDesc().Format,
        dxManager.GetsrvDescriptorHeap(),
        dxManager.GetsrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
        dxManager.GetsrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
    );
    item_current = 0;

    // カメラ系
    cameraTransform = { {1.0f,1.0f,1.0f}, {0.3f,0.0f,0.0f}, {0.0f,4.0f,-10.0f} };

    // 読み込んだオブジェクトの合計
    objectSum = 0;
    textureSum = 0;

    // 光源の設定
    directionalLightResource = CreateBufferResource(dxManager.GetDevice(), sizeof(DirectionalLigft));
    directionalLightData = nullptr;
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData->direction = Normalize({ 0.0f, -1.0f, 0.0f });
    directionalLightData->intensity = 1.0f;

    // リソース読み込み
    uvCheckerTex = LoadTexture("resources/uvChecker.png");
    monsterBallTex = LoadTexture("resources/monsterBall.png");
    gold1x1Tex = LoadTexture("resources/gold1x1.png");

    obj1 = LoadOBJ("resources", "axis.obj");
    obj2 = LoadOBJ("resources", "plane.obj");
    obj3 = LoadOBJ("resources", "multiMaterial.obj");
    obj4 = LoadOBJ("resources", "multiMesh.obj");
}

Game::~Game()
{
    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // COMの終了処理
    CoUninitialize();
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
            ImGuiUpdate();
            Update();
            Render();
        }
    }
}

void Game::Update()
{
    // ゲームロジックの更新

    // ライトの向きを正規化
    directionalLightData->direction = Normalize(directionalLightData->direction);

    // カメラの設定
    Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
    viewMatrix = Inverse(cameraMatrix);
    projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);// int width, int heightをもってくる



}

void Game::Render()
{
    dxManager.BeginFrame();

    Drawobj(transformOBJ1, obj1, uvCheckerTex, 0);
    Drawobj(transformOBJ2, obj1, gold1x1Tex, 1);




    dxManager.EndFrame();
}

void Game::ImGuiUpdate()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("CG2_02");

    if (ImGui::CollapsingHeader("camera"))
    {
        ImGui::DragFloat3("CameraScale", &cameraTransform.scale.x, 0.01f);
        ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 0.01f);
        ImGui::DragFloat3("CameraTranslate", &cameraTransform.translate.x, 0.01f);
        ImGui::SliderAngle("CameraRotateX", &cameraTransform.rotate.x);
        ImGui::SliderAngle("CameraRotateY", &cameraTransform.rotate.y);
        ImGui::SliderAngle("CameraRotateZ", &cameraTransform.rotate.z);
    }

    const char* items[] = { "axis.obj", "plane.obj" };
    if (ImGui::CollapsingHeader("object", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Combo("Combo Box", &item_current, items, IM_ARRAYSIZE(items));
        ImGui::ColorEdit3("ObjectColor", (float*)&objects[item_current].materialData->color.x);
        ImGui::DragFloat3("ObjectScale", &objects[item_current].transform.scale.x, 0.01f);
        ImGui::DragFloat3("ObjectRotate", &objects[item_current].transform.rotate.x, 0.01f);
        ImGui::DragFloat3("ObjectTranslate", &objects[item_current].transform.translate.x, 0.01f);
        ImGui::SliderAngle("ObjectRotateX", &objects[item_current].transform.rotate.x);
        ImGui::SliderAngle("ObjectRotateY", &objects[item_current].transform.rotate.y);
        ImGui::SliderAngle("ObjectRotateZ", &objects[item_current].transform.rotate.z);
        ImGui::Checkbox("X", &autoRotation[0]);
        ImGui::SameLine(0.0f, 54.0f);
        ImGui::Checkbox("Y", &autoRotation[1]);
        ImGui::SameLine(0.0f, 54.0f);
        ImGui::Checkbox("Z", &autoRotation[2]);
        ImGui::SameLine(0.0f, 54.0f);
        ImGui::Text("AutoRotation");


        ImGui::DragFloat3("transformOBJ1Scale", &transformOBJ1.scale.x, 0.01f);
        ImGui::DragFloat3("transformOBJ1Rotate", &transformOBJ1.rotate.x, 0.01f);
        ImGui::DragFloat3("transformOBJ1Translate", &transformOBJ1.translate.x, 0.01f);

        ImGui::DragFloat3("transformOBJ2Scale", &transformOBJ2.scale.x, 0.02f);
        ImGui::DragFloat3("transformOBJ2Rotate", &transformOBJ2.rotate.x, 0.02f);
        ImGui::DragFloat3("transformOBJ2Translate", &transformOBJ2.translate.x, 0.02f);
    }
    if (autoRotation[0])objects[item_current].transform.rotate.x += 0.01f;
    if (autoRotation[1])objects[item_current].transform.rotate.y += 0.01f;
    if (autoRotation[2])objects[item_current].transform.rotate.z += 0.01f;

    //if (ImGui::CollapsingHeader("sprite"))
    //{
    //    ImGui::ColorEdit3("SpriteColor", (float*)&materialDataSprite->color.x);
    //    ImGui::DragFloat2("SpriteScale", &transformSprite.scale.x, 0.01f);
    //    ImGui::DragFloat2("SpriteRotate", &transformSprite.rotate.x, 0.01f);
    //    ImGui::DragFloat2("SpriteTranslate", &transformSprite.translate.x, 1.0f);
    //}

    //if (ImGui::CollapsingHeader("uv"))
    //{
    //    ImGui::DragFloat2("uvScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
    //    ImGui::SliderAngle("uvRotate", &uvTransformSprite.rotate.z);
    //    ImGui::DragFloat2("uvTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
    //}

    if (ImGui::CollapsingHeader("light"))
    {
        ImGui::ColorEdit3("LightColor", (float*)&directionalLightData->color.x);
        ImGui::DragFloat3("LightDirection", &directionalLightData->direction.x, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat("LightIntensity", &directionalLightData->intensity, 0.01f);
    }

    ImGui::End();
    ImGui::Render();

}


void Game::Drawobj(const Transforms& localTransform, uint32_t objectNumeber, uint32_t textureNumber, size_t matrixIndex)
{
    Object3D& obj = objects[objectNumeber];

    // objectNumeberの等しいオブジェクトの描画が２回目以降になったらransformationMatrixを拡張する（objectNumeberが等しい＝objects[].transformを共有しているから各々独立させて動かすことが出来ないから）
    // じゃあobjects[].transformいらなくない？　→　objects[].transformはobjectNumeberが等しいモデル全てに影響を及ぼすtransformとしてつかえるんじゃよそれはそれで使い道がありそうじゃろう
    if (matrixIndex >= obj.transformationMatrixResource.size()) {
        size_t oldSize = obj.transformationMatrixResource.size();
        obj.transformationMatrixResource.resize(matrixIndex + 1);
        obj.transformationMatrixData.resize(matrixIndex + 1);
        for (size_t i = oldSize; i <= matrixIndex; ++i) {
            obj.transformationMatrixResource[i] = CreateBufferResource(dxManager.GetDevice(), sizeof(TransformationMatrix));
            obj.transformationMatrixData[i] = nullptr;
            obj.transformationMatrixResource[i]->Map(0, nullptr, reinterpret_cast<void**>(&obj.transformationMatrixData[i]));
            obj.transformationMatrixData[i]->World = MakeIdentity4x4();
            obj.transformationMatrixData[i]->WVP = MakeIdentity4x4();
            obj.transformationMatrixResource[i]->Unmap(0, nullptr);
        }
    }

    // オブジェクトのWorldViewProjectionMatrixを作る
    Matrix4x4 objectMatrix = MakeAffineMatrix(objects[objectNumeber].transform.scale, objects[objectNumeber].transform.rotate, objects[objectNumeber].transform.translate);
    Matrix4x4 drawMatrix = MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate);

    obj.transformationMatrixResource[matrixIndex]->Map(0, nullptr, reinterpret_cast<void**>(&obj.transformationMatrixData[matrixIndex]));
    obj.transformationMatrixData[matrixIndex]->World = Mul(objectMatrix, drawMatrix);
    obj.transformationMatrixData[matrixIndex]->WVP = Mul(obj.transformationMatrixData[matrixIndex]->World, Mul(viewMatrix, projectionMatrix));
    obj.transformationMatrixResource[matrixIndex]->Unmap(0, nullptr);

    // textureNumberに一致するテクスチャを探す
    const textureData* tex = nullptr;
    for (const auto& t : textures) {
        if (t.number == textureNumber) {
            tex = &t;
            break;
        }
    }
    //assert(tex && "指定されたtextureNumberのテクスチャが見つかりません");
    if (tex == nullptr)
    {
        tex = &textures[0];
    }

    // 描画処理
    dxManager.GetCommandList()->IASetVertexBuffers(0, 1, &objects[objectNumeber].vertexBufferView);
    // 形状を設定
    dxManager.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager.GetCommandList()->SetGraphicsRootConstantBufferView(0, objects[objectNumeber].materialResource->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager.GetCommandList()->SetGraphicsRootConstantBufferView(1, objects[objectNumeber].transformationMatrixResource[matrixIndex]->GetGPUVirtualAddress());
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager.GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());



    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager.GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);

    // 描画
    dxManager.GetCommandList()->DrawInstanced(UINT(objects[objectNumeber].modelData.vertices.size()), 1, 0, 0);
}

int Game::LoadTexture(const std::string& filePath)
{
    // ボックスを作成
    textureData text;

    // テクスチャファイルを読んでプログラムを扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathw = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathw.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    DirectX::ScratchImage mipImageLocal;
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImageLocal);
    assert(SUCCEEDED(hr));
    
    text.metadata = mipImageLocal.GetMetadata();
    text.number = textureSum;
    text.mipImage = std::move(mipImageLocal);
    textureSum++;


    // テクスチャリソースとSRVの作成
    text.textureResource = CreateTextureResource(dxManager.GetDevice(), text.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, dxManager.GetDevice(), dxManager.GetCommandList());


    const uint32_t descriptorSizeSRV = dxManager.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(dxManager.GetsrvDescriptorHeap(), descriptorSizeSRV, textureSum);
    text.textureSrvHandleGPU = GetGPUDescriptorHandle(dxManager.GetsrvDescriptorHeap(), descriptorSizeSRV, textureSum);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = text.metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(text.metadata.mipLevels);

    dxManager.GetDevice()->CreateShaderResourceView(text.textureResource.Get(), &srvDesc, textureSrvHandleCPU);

    textures.push_back(std::move(text));

    return text.number;
}

int Game::LoadOBJ(const std::string& directoryPath, const std::string& filename)
{
    // ボックスを作成
    Object3D obj;

    // モデルデータ
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
    obj.transformationMatrixResource.resize(1);
    obj.transformationMatrixData.resize(1);
    obj.transformationMatrixResource[0] = CreateBufferResource(dxManager.GetDevice(), sizeof(TransformationMatrix));
    obj.transformationMatrixData[0] = nullptr;
    obj.transformationMatrixResource[0]->Map(0, nullptr, reinterpret_cast<void**>(&obj.transformationMatrixData[0]));
    obj.transformationMatrixData[0]->World = MakeIdentity4x4();
    obj.transformationMatrixData[0]->WVP = MakeIdentity4x4();
    obj.transformationMatrixResource[0]->Unmap(0, nullptr);

    // 識別ナンバーの設定
    obj.number = objectSum;
    objectSum++;

    objects.push_back(obj);

    return obj.number;
}


