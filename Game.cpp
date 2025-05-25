#include "Game.h"
#include "functions.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cstdint>

WindowManager* Game::windowManager = nullptr;
DirectXManager* Game::dxManager = nullptr;

std::vector<Object3D> Game::objects;
uint32_t Game::objectSum = 0;

std::vector<TextureData> Game::textures;
uint32_t Game::textureSum = 0;

Microsoft::WRL::ComPtr<ID3D12Resource> Game::vertexResourceSprite;
UINT Game::vertexResourceSizeSprite;
Microsoft::WRL::ComPtr<ID3D12Resource> Game::vertexResourceObj;
UINT Game::vertexResourceSizeObj;
Microsoft::WRL::ComPtr<ID3D12Resource> Game::vertexResourceTriangle;
UINT Game::vertexResourceSizeTriangle;
Microsoft::WRL::ComPtr<ID3D12Resource> Game::vertexResourceSphere;
UINT Game::vertexResourceSizeSphere;
Microsoft::WRL::ComPtr<ID3D12Resource> Game::indexResource;
D3D12_INDEX_BUFFER_VIEW Game::indexBufferView;

std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> Game::materialResources;
std::vector<Material*> Game::materialData;
std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> Game::wvpResources;
std::vector<TransformationMatrix*> Game::wvpData;
size_t Game::drawCallIndex = 0;

Microsoft::WRL::ComPtr<ID3D12Resource> Game::directionalLightResource;
DirectionalLight* Game::directionalLightData = nullptr;

CameraController* Game::cameraController;

int Game::wheelDelta = 0;

// 初期化用
void Game::Initialize(int width, int height, const std::wstring& title)
{
    // COM の初期化
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));
    // 例外ハンドラの設定
    SetUnhandledExceptionFilter(ExportDump);

    if (!windowManager) {
        windowManager = new WindowManager(width, height, title);
    }
    if (!dxManager) {
        dxManager = new DirectXManager(windowManager->GetHwnd(), width, height);
    }

    /// imguiの初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(windowManager->GetHwnd());
    ImGui_ImplDX12_Init(
        dxManager->GetDevice(),
        dxManager->GetSwapChainDesc().BufferCount,
        dxManager->GetRtvDesc().Format,
        dxManager->GetsrvDescriptorHeap(),
        dxManager->GetsrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
        dxManager->GetsrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
    );

    // カメラ系
    cameraController = new CameraController;


    // 読み込んだオブジェクトの合計
    objectSum = 0;
    textureSum = 1;

    // 頂点リソース
    vertexResourceSizeSprite = sizeof(VertexData) * 256; // スプライト
    vertexResourceSprite = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeSprite);

    vertexResourceSizeObj = sizeof(VertexData) * 4096; // オブジェクト
    vertexResourceObj = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeObj);

    vertexResourceSizeTriangle = sizeof(VertexData) * 1024; // 三角形
    vertexResourceTriangle = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeTriangle);

    vertexResourceSizeSphere = sizeof(VertexData) * 4096; // 球
    vertexResourceSphere = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeSphere);

    materialResources.resize(kMaxDrawCallPerFrame);
    materialData.resize(kMaxDrawCallPerFrame);
    wvpResources.resize(kMaxDrawCallPerFrame);
    wvpData.resize(kMaxDrawCallPerFrame);
    for (size_t i = 0; i < kMaxDrawCallPerFrame; ++i) {
        materialResources[i] = CreateBufferResource(dxManager->GetDevice(), sizeof(Material));
        materialResources[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialData[i]));
        wvpResources[i] = CreateBufferResource(dxManager->GetDevice(), sizeof(TransformationMatrix));
        wvpResources[i]->Map(0, nullptr, reinterpret_cast<void**>(&wvpData[i]));
    }

    // インデックスリソース
    indexResource = CreateBufferResource(dxManager->GetDevice(), sizeof(uint32_t) * 6);
    uint32_t* indexData = nullptr;
    indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    indexData[0] = 0;
    indexData[1] = 1;
    indexData[2] = 2;
    indexData[3] = 1;
    indexData[4] = 3;
    indexData[5] = 2;
    indexResource->Unmap(0, nullptr);

    // リソースの先頭のアドレスから使う
    indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
    // 仕様するリソースのサイズはインデックス６つ分のサイズ
    indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
    // インデックスはuint32_tとする
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;


    // 光源の設定
    directionalLightResource = CreateBufferResource(dxManager->GetDevice(), sizeof(DirectionalLight));
    directionalLightData = nullptr;
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData->direction = Normalize({ 0.0f, -1.0f, 0.0f });
    directionalLightData->intensity = 1.0f;
}

// メインループ用
bool Game::ProcessMessage() {
    MSG msg = {};
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        if (msg.message == WM_MOUSEWHEEL) {
            // ホイールの回転量を加算　クリックはboolで回転量はintだからmessageを使う。らしい。
            wheelDelta += GET_WHEEL_DELTA_WPARAM(msg.wParam);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}
void Game::BeginFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    UpdateCameraAndLight();
    dxManager->BeginFrame();
}
void Game::UpdateCameraAndLight()
{
    // ライトの向きを正規化
    directionalLightData->direction = Normalize(directionalLightData->direction);

    // カメラの更新
    cameraController->Updata();
}
void Game::EndFrame()
{
    ImGui::Render();

    dxManager->EndFrame();

    for (uint32_t objectNum = 0; objectNum < objectSum; ++objectNum)
    {
        objects[objectNum].drawCount = 0;
    }
    drawCallIndex = 0;
}

// 終了処理
void Game::Finalize()
{
    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // COMの終了処理
    CoUninitialize();

    // 解放処理
    delete dxManager;
    dxManager = nullptr;
    delete windowManager;
    windowManager = nullptr;
    delete cameraController;
    cameraController = nullptr;
}




// リソース読み込み
int Game::LoadTexture(const std::string& filePath)
{
    // ボックスを作成
    TextureData text;

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
    text.textureResource = CreateTextureResource(dxManager->GetDevice(), text.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, dxManager->GetDevice(), dxManager->GetCommandList());


    const uint32_t descriptorSizeSRV = dxManager->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(dxManager->GetsrvDescriptorHeap(), descriptorSizeSRV, textureSum);
    text.textureSrvHandleGPU = GetGPUDescriptorHandle(dxManager->GetsrvDescriptorHeap(), descriptorSizeSRV, textureSum);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = text.metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(text.metadata.mipLevels);

    dxManager->GetDevice()->CreateShaderResourceView(text.textureResource.Get(), &srvDesc, textureSrvHandleCPU);

    textures.push_back(std::move(text));

    return text.number;
}

int Game::LoadOBJ(const std::string& directoryPath, const std::string& filename)
{
    // ボックスを作成
    Object3D obj;

    // モデルデータ
    obj.modelData = LoadOBJFile(directoryPath, filename);


    // マテリアルデータ
    //obj.materialResource = CreateBufferResource(dxManager->GetDevice(), sizeof(Material));
    //obj.materialData = nullptr;
    //obj.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&obj.materialData));
    //obj.materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    //obj.materialData->enableLighting = true;
    //obj.materialData->uvTransform = MakeIdentity4x4();
    //obj.materialResource->Unmap(0, nullptr);

    // 変換行列
    obj.transform = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

    // ワールド・ビュー・プロジェクション行列
    //obj.transformationMatrixResource.resize(1);
    //obj.transformationMatrixData.resize(1);
    //obj.transformationMatrixResource[0] = CreateBufferResource(dxManager->GetDevice(), sizeof(TransformationMatrix));
    //obj.transformationMatrixData[0] = nullptr;
    //obj.transformationMatrixResource[0]->Map(0, nullptr, reinterpret_cast<void**>(&obj.transformationMatrixData[0]));
    //obj.transformationMatrixData[0]->World = MakeIdentity4x4();
    //obj.transformationMatrixData[0]->WVP = MakeIdentity4x4();
    //obj.transformationMatrixResource[0]->Unmap(0, nullptr);

    // 識別ナンバーの設定
    obj.number = objectSum;
    objectSum++;

    // 表示回数初期化
    obj.drawCount = 0;

    // ボックスをpush_back
    objects.push_back(obj);

    // 識別ナンバーをreturn
    return obj.number;
}

// 描画
void Game::Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const Vector4& materialColor)
{
    // 描画回数上限
    if (drawCallIndex >= kMaxDrawCallPerFrame) return;

    // ボックスの作成
    Object3D& obj = objects[objectNumber];

    // 必要な頂点数
    const uint32_t vertexCount = static_cast<uint32_t>(obj.modelData.vertices.size());
    if (vertexCount == 0) return;
    if (sizeof(VertexData) * vertexCount > vertexResourceSizeObj) return;

    // 頂点リソース
    VertexData* vData = nullptr;
    HRESULT hr = vertexResourceObj->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    if (FAILED(hr) || vData == nullptr) return;
    std::memcpy(vData, obj.modelData.vertices.data(), sizeof(VertexData) * vertexCount);
    vertexResourceObj->Unmap(0, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = vertexResourceObj->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexCount;
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    // マテリアル
    materialData[drawCallIndex]->color = materialColor;
    materialData[drawCallIndex]->enableLighting = true;
    materialData[drawCallIndex]->uvTransform = MakeIdentity4x4();

    // wvp
    Matrix4x4 toCenter = MakeTranslateMatrix({ -center.x, -center.y, -center.z });          // centerへ移動用マトリックス
    Matrix4x4 rotateScale = MakeAffineMatrix(transform.scale, transform.rotate, { 0,0,0 }); // 回転・スケール用マトリックス
    Matrix4x4 fromCenter = MakeTranslateMatrix(center);                                     // centerから元の位置へ戻す用マトリックス
    Matrix4x4 translate = MakeTranslateMatrix(transform.translate);                         // 移動用マトリックス


    // １、Mul(rotateScale, toCenter) = 回転中心へ移動してから回転拡縮。center = transform.translate ならその場で回る
    // ２、Mul(fromCenter, ↑)        = 回転中心から元の位置へ戻す
    // ３、Mul(translate, ↑)         = 最終的な平行移動（全体の移動）
    // ４、centerを中心に回転拡縮し、最後にtransform.translateで移動した結果のマトリックスが完成
    Matrix4x4 worldMatrix = Mul(translate, Mul(fromCenter, Mul(rotateScale, toCenter)));

    // オブジェクト間共有マトリックス
    Matrix4x4 objectMatrix = MakeAffineMatrix(obj.transform.scale, obj.transform.rotate, obj.transform.translate);

    wvpData[drawCallIndex]->World = Mul(objectMatrix, worldMatrix);
    wvpData[drawCallIndex]->WVP = Mul(wvpData[drawCallIndex]->World, cameraController->viewProjectionMatrix);


    // textureNumberに一致するテクスチャを探す
    const TextureData* tex = nullptr;
    for (const auto& t : textures) {
        if (t.number == textureNumber) {
            tex = &t;
            break;
        }
    }
    // 見つからなかったらuncheckを使う
    if (tex == nullptr) {
        tex = &textures[0];
    }

    // 描画処理
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    // 形状を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    //dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, obj.materialResource->GetGPUVirtualAddress());
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    //dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, obj.transformationMatrixResource[obj.drawCount]->GetGPUVirtualAddress());
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());


    // 描画
    dxManager->GetCommandList()->DrawInstanced(vertexCount, 1, 0, 0);

    // 描画回数更新
    obj.drawCount += 1;
}

void Game::DrawTriangle(const Transforms& localTransform, const Transforms& worldTransform, const VertexData* vertexData, uint32_t kSumVertex, uint32_t textureNumber, const Vector4& materialColor)
{
    if (kSumVertex * sizeof(VertexData) > vertexResourceSizeTriangle) return;

    VertexData* vData = nullptr;
    HRESULT hr = vertexResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    if (FAILED(hr) || vData == nullptr) return;
    std::memcpy(vData, vertexData, sizeof(VertexData) * kSumVertex);
    vertexResourceTriangle->Unmap(0, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = vertexResourceTriangle->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(VertexData) * kSumVertex;
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    const TextureData* tex = nullptr;
    for (const auto& t : textures) {
        if (t.number == textureNumber) {
            tex = &t;
            break;
        }
    }
    if (tex == nullptr) {
        tex = &textures[0];
    }


    // マテリアルリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(dxManager->GetDevice(), sizeof(Material));
    // マテリアルにデータを書き込む
    Material* materialData = nullptr;
    // 書き込むためのアドレスを取得
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    // 今回は赤を書き込んでみる
    materialData->color = materialColor;
    materialData->enableLighting = true;
    materialData->uvTransform = MakeIdentity4x4();
    materialResource->Unmap(0, nullptr);

    // World-View-Projection用のリソースを作る。TransformationMatrix　１つ分のサイズを用意する
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(dxManager->GetDevice(), sizeof(TransformationMatrix));
    // データを書き込む
    TransformationMatrix* wvpData = nullptr;
    // 書き込むためのアドレスを取得
    hr = wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    // 単位行列を書き込んでおく
    wvpData->World = MakeIdentity4x4();
    wvpData->WVP = MakeIdentity4x4();
    if (SUCCEEDED(hr) && wvpData) {
        wvpData->World = Mul(MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate), MakeAffineMatrix(worldTransform.scale, worldTransform.rotate, worldTransform.translate));
        wvpData->WVP = Mul(wvpData->World, cameraController->viewProjectionMatrix);
        wvpResource->Unmap(0, nullptr);
    }

   
   
    
   

    // RootSignatureを設定。
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    // 形状を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());


    // 描画
    dxManager->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);
}

void Game::DrawSphere(const Transforms& localTransform, VertexData* vertexData, uint32_t kSubdivision, uint32_t textureNumber, const Vector4& materialColor)
{
    const uint32_t kSumVertex = kSubdivision * kSubdivision * 6;
    if (kSumVertex * sizeof(VertexData) > vertexResourceSizeSphere) return;

    CreateSphere(vertexData, kSubdivision);

    VertexData* vData = nullptr;
    HRESULT hr = vertexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    if (FAILED(hr) || vData == nullptr) return;
    std::memcpy(vData, vertexData, sizeof(VertexData) * kSumVertex);
    vertexResourceSphere->Unmap(0, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(VertexData) * kSumVertex;
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    const TextureData* tex = nullptr;
    for (const auto& t : textures) {
        if (t.number == textureNumber) {
            tex = &t;
            break;
        }
    }
    if (tex == nullptr) {
        tex = &textures[0];
    }

    // マテリアルリソースを作成
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(dxManager->GetDevice(), sizeof(Material));
    Material* materialData = nullptr;
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    materialData->color = materialColor;
    materialData->enableLighting = true;
    materialData->uvTransform = MakeIdentity4x4();
    materialResource->Unmap(0, nullptr);

    // WVPリソース（単位行列）を作成
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(dxManager->GetDevice(), sizeof(TransformationMatrix));
    TransformationMatrix* wvpData = nullptr;
    hr = wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    wvpData->World = MakeIdentity4x4();
    wvpData->WVP = MakeIdentity4x4();
    if (SUCCEEDED(hr) && wvpData) {
        wvpData->World = MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate);
        wvpData->WVP = Mul(wvpData->World, cameraController->viewProjectionMatrix);
        wvpResource->Unmap(0, nullptr);
    }

    // 描画処理
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    // 形状を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

    dxManager->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

}

void Game::DrawSprite(const Transforms& localTransform, VertexData* vertexData, uint32_t textureNumber, const Vector4& materialColor)
{
    VertexData* vData = nullptr;
    HRESULT hr = vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    if (FAILED(hr) || vData == nullptr) return;
    std::memcpy(vData, vertexData, sizeof(VertexData) * 4);
    vertexResourceSprite->Unmap(0, nullptr);


    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    // リソースの先頭のアドレスから使う
    vertexBufferView.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
    // 仕様するリソースのサイズは頂点4つ分のサイズ
    vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
    // １頂点あたりのサイズ
    vertexBufferView.StrideInBytes = sizeof(VertexData);


    // textureNumberに一致するテクスチャを探す
    const TextureData* tex = nullptr;
    for (const auto& t : textures) {
        if (t.number == textureNumber) {
            tex = &t;
            break;
        }
    }
    // 見つからなかったらuncheckを使う
    if (tex == nullptr) {
        tex = &textures[0];
    }

    // マテリアルリソースを作成
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(dxManager->GetDevice(), sizeof(Material));
    Material* materialData = nullptr;
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    materialData->color = materialColor;
    materialData->enableLighting = false;
    materialData->uvTransform = MakeIdentity4x4();
    materialResource->Unmap(0, nullptr);

    // WVPリソース（単位行列）を作成
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(dxManager->GetDevice(), sizeof(TransformationMatrix));
    TransformationMatrix* wvpData = nullptr;
    hr = wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    wvpData->World = MakeIdentity4x4();
    wvpData->WVP = MakeIdentity4x4();
    if (SUCCEEDED(hr) && wvpData) {
        Matrix4x4 orthoProjectionMatrix =
            MakeOrthographicMatrix(
                0.0f,
                0.0f,
                static_cast<float>(windowManager->Getwidth()),
                static_cast<float>(windowManager->Getheight()),
                0.0f,
                100.0f
            );
        // ワールド行列と正射影行列を乗算してWVP行列を計算
        wvpData->World = MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate);
        wvpData->WVP = Mul(wvpData->World, orthoProjectionMatrix); // または Mul(worldMatrix, MakeIdentity4x4() * orthoProjectionMatrix);
        wvpResource->Unmap(0, nullptr);
    }

    // Spriteの描画
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
    dxManager->GetCommandList()->IASetIndexBuffer(&indexBufferView);
    // 形状を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

    // 描画
    dxManager->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

// 入力
void Game::GetMousePosition(Vector2* position)
{
    // hwnd: ゲームウィンドウのハンドル（WindowManagerなどから取得）
    POINT mousePosScreen;
    GetCursorPos(&mousePosScreen); // 画面座標で取得

    // クライアント座標（ウィンドウ左上基準）に変換
    ScreenToClient(windowManager->GetHwnd(), &mousePosScreen);

    // mousePosScreen.x, mousePosScreen.y がウィンドウ内のマウス座標
    position->x = float(mousePosScreen.x);
    position->y = float(mousePosScreen.y);
}

bool Game::IsPressMouse(int i)
{
    // 左クリック
    if (i == 0)
    {
        bool leftButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        return leftButton;
    }
    // 右クリック
    if (i == 1)
    {
        bool rightButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        return rightButton;
    }

    return false;
}

int Game::GetWheel()
{
    int delta = wheelDelta;
    wheelDelta = 0; // 1フレームで消費
    return delta;
}

