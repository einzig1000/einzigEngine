#include "Game.h"
#include "functions.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <cstdint>

WindowManager* Game::windowManager = nullptr;
DirectXManager* Game::dxManager = nullptr;

std::vector<Object3D> Game::objects;

std::vector<TextureData> Game::textures;

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

    // 頂点リソース
    vertexResourceSizeSprite = static_cast<UINT>(sizeof(VertexData) * 256); // スプライト 
    vertexResourceSprite = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeSprite);

    vertexResourceSizeObj = static_cast<UINT>(sizeof(VertexData) * 4096); // オブジェクト
    vertexResourceObj = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeObj);

    vertexResourceSizeTriangle = static_cast<UINT>(sizeof(VertexData) * 1024); // 三角形
    vertexResourceTriangle = CreateBufferResource(dxManager->GetDevice(), vertexResourceSizeTriangle);

    vertexResourceSizeSphere = static_cast<UINT>(sizeof(VertexData) * 4096); // 球
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

// Draw用データ作成するやつ
DrawData Game::SetupDrawData(size_t dstBufferSize, const VertexData* srcVertexData, size_t vertexCount, Microsoft::WRL::ComPtr<ID3D12Resource>& vertexResource, UINT& vertexResourceSize, Material* material, const Vector4& materialColor, bool enableLighting, const Matrix4x4& uvTransform, TransformationMatrix* wvp, const Matrix4x4& world, const Matrix4x4& wvpMatrix, uint32_t textureNumber, const std::vector<TextureData>& textures)
{
    // 描画回数上限
    if (drawCallIndex >= kMaxDrawCallPerFrame) return{};

    // 頂点数
    if (vertexCount == 0) return{};
    if (vertexCount * sizeof(VertexData) > dstBufferSize) return {};

    // 頂点リソース
    VertexData* vData = nullptr;
    HRESULT hr = vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    if (FAILED(hr) || vData == nullptr) return {};
    std::memcpy(vData, srcVertexData, sizeof(VertexData) * vertexCount);
    vertexResource->Unmap(0, nullptr);

    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(vertexCount);
    vertexBufferView.StrideInBytes = sizeof(VertexData);

    // マテリアル
    material->color = materialColor;
    material->enableLighting = enableLighting;
    material->uvTransform = uvTransform;

    // WVP
    wvp->World = world;
    wvp->WVP = wvpMatrix;

    // テクスチャ
    const TextureData* tex = nullptr;
    for (const auto& t : textures)
    {
        if (t.number == textureNumber)
        {
            tex = &t;
            break;
        }
    }
    if (tex == nullptr && !textures.empty())
    {
        tex = &textures[0];
    }

    return { vertexBufferView, tex };
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
    text.number = static_cast<uint32_t> (textures.size());
    text.mipImage = std::move(mipImageLocal);


    // テクスチャリソースとSRVの作成
    text.textureResource = CreateTextureResource(dxManager->GetDevice(), text.metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(text.textureResource.Get(), text.mipImage, dxManager->GetDevice(), dxManager->GetCommandList());


    const uint32_t descriptorSizeSRV = dxManager->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(dxManager->GetsrvDescriptorHeap(), descriptorSizeSRV, text.number+1);
    text.textureSrvHandleGPU = GetGPUDescriptorHandle(dxManager->GetsrvDescriptorHeap(), descriptorSizeSRV, text.number+1);

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

    // 変換行列
    obj.transform = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

    // 識別ナンバーの設定
    obj.number = static_cast<uint32_t>(objects.size());

    // ボックスをpush_back
    objects.push_back(obj);

    // 識別ナンバーをreturn
    return obj.number;
}

// 描画
void Game::Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const Vector4& materialColor)
{
    // objectNumberの範囲チェック
    if (objectNumber >= objects.size()) return;

    // ボックスの作成
    Object3D& obj = objects[objectNumber];

    // 必要な頂点数
    const uint32_t kSumVertex = static_cast<uint32_t>(obj.modelData.vertices.size());

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

    Matrix4x4 world = Mul(objectMatrix, worldMatrix);
    Matrix4x4 wvpMatrix = Mul(world, cameraController->viewProjectionMatrix);

    // 共通セットアップ
    DrawData drawData = SetupDrawData(
        vertexResourceSizeObj,
        obj.modelData.vertices.data(),
        kSumVertex,
        vertexResourceObj,
        vertexResourceSizeObj,
        materialData[drawCallIndex],
        materialColor,
        true,
        MakeIdentity4x4(),
        wvpData[drawCallIndex],
        world,
        wvpMatrix,
        textureNumber,
        textures
    );
    if (!drawData.texture) return;

    // 描画処理
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &drawData.vertexBufferView);
    // 形状を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, drawData.texture->textureSrvHandleGPU);
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());


    // 描画
    dxManager->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

    drawCallIndex++;
}

void Game::DrawTriangle(const Transforms& localTransform, const Transforms& worldTransform, const VertexData* vertexData, uint32_t textureNumber, const Vector4& materialColor)
{
    Matrix4x4 world = Mul(
        MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate),
        MakeAffineMatrix(worldTransform.scale, worldTransform.rotate, worldTransform.translate)
    );
    Matrix4x4 wvpMatrix = Mul(world, cameraController->viewProjectionMatrix);

    DrawData drawData = SetupDrawData(
        vertexResourceSizeTriangle,
        vertexData,
        3,
        vertexResourceTriangle,
        vertexResourceSizeTriangle,
        materialData[drawCallIndex],
        materialColor,
        true,
        MakeIdentity4x4(),
        wvpData[drawCallIndex],
        world,
        wvpMatrix,
        textureNumber,
        textures
    );
    if (!drawData.texture) return;
   

    // RootSignatureを設定。
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &drawData.vertexBufferView);
    // 形状を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, drawData.texture->textureSrvHandleGPU);
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

    // 描画
    dxManager->GetCommandList()->DrawInstanced(3, 1, 0, 0);

    drawCallIndex++;
}

void Game::DrawSphere(const Transforms& localTransform, VertexData* vertexData, uint32_t kSubdivision, uint32_t textureNumber, const Vector4& materialColor)
{
    // 必要な頂点数
    const uint32_t kSumVertex = kSubdivision * kSubdivision * 6;

    // 頂点
    CreateSphere(vertexData, kSubdivision);

    Matrix4x4 world = MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate);
    Matrix4x4 wvpMatrix = Mul(world, cameraController->viewProjectionMatrix);

    DrawData drawData = SetupDrawData(
        vertexResourceSizeSphere,
        vertexData,
        kSumVertex,
        vertexResourceSphere,
        vertexResourceSizeSphere,
        materialData[drawCallIndex],
        materialColor,
        true,
        MakeIdentity4x4(),
        wvpData[drawCallIndex],
        world,
        wvpMatrix,
        textureNumber,
        textures
    );
    if (!drawData.texture) return;


    // 描画処理
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &drawData.vertexBufferView);
    // 形状を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, drawData.texture->textureSrvHandleGPU);
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

    dxManager->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

    drawCallIndex++;
}

void Game::DrawSprite(const Transforms& localTransform, VertexData* vertexData, uint32_t textureNumber, const Vector4& materialColor)
{
    Matrix4x4 orthoProjectionMatrix = MakeOrthographicMatrix(
        0.0f, 0.0f,
        static_cast<float>(windowManager->Getwidth()),
        static_cast<float>(windowManager->Getheight()),
        0.0f, 100.0f);
    Matrix4x4 world = MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate);
    Matrix4x4 wvpMatrix = Mul(world, orthoProjectionMatrix);

    DrawData drawData = SetupDrawData(
        vertexResourceSizeSprite,
        vertexData,
        4,
        vertexResourceSprite,
        vertexResourceSizeSprite,
        materialData[drawCallIndex],
        materialColor,
        false,
        MakeIdentity4x4(),
        wvpData[drawCallIndex],
        world,
        wvpMatrix,
        textureNumber,
        textures
    );

    // Spriteの描画
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &drawData.vertexBufferView);
    dxManager->GetCommandList()->IASetIndexBuffer(&indexBufferView);
    // 形状を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // CBVを設定する マテリアル用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
    // CBVを設定する wvp用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, drawData.texture->textureSrvHandleGPU);
    // CBVを設定する ディレクショナルライト用のCBufferの場所を設定
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

    // 描画
    dxManager->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

    drawCallIndex++;
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

