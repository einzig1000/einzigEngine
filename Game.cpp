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
MouseController* Game::mouseController;

int Game::wheelDelta = 0;

// 初期化用
void Game::Initialize(int width, int height, const std::wstring& title)
{
    // COM の初期化
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));
    // 例外ハンドラの設定
    SetUnhandledExceptionFilter(ExportDump);

    if (!windowManager)
    {
        windowManager = new WindowManager(width, height, title);
    }
    if (!dxManager)
    {
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

    // カメラ
    cameraController = new CameraController;

    // マウス
    mouseController = new MouseController;

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
    for (size_t i = 0; i < kMaxDrawCallPerFrame; ++i)
    {
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
    directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
    directionalLightData->intensity = 1.0f;
}

// メインループ用
bool Game::ProcessMessage()
{
    MSG msg = {};
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }
        if (msg.message == WM_MOUSEWHEEL)
        {
            // ホイールの回転量を加算　クリックはboolで回転量はintだからmessageを使う。らしい。なんで？
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


    UpdateLight();
    UpdateCamera();
    dxManager->BeginFrame();
}
void Game::UpdateLight()
{
    // ライトの向きを正規化
    directionalLightData->direction = (directionalLightData->direction.Normalized());
}
void Game::UpdateCamera()
{
    if (GetHitKey::keys[DIK_SPACE] && !GetHitKey::preKeys[DIK_SPACE])
    {
        if (cameraController->cameraMode_ == 1)cameraController->cameraMode_ = 0;
        else cameraController->cameraMode_ = 1;
    }
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
    delete mouseController;
    mouseController = nullptr;
}

// Draw用データ作成するやつ
DrawData Game::SetupDrawData(size_t dstBufferSize, const VertexData* srcVertexData, size_t vertexCount, Microsoft::WRL::ComPtr<ID3D12Resource>& vertexResource, UINT& vertexResourceSize, Material* material, const uint32_t& materialColor, bool enableLighting, const Matrix4x4& uvTransform, TransformationMatrix* wvp, const Matrix4x4& world, const Matrix4x4& wvpMatrix, uint32_t textureNumber, const std::vector<TextureData>& textures)
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
    Vector4 color = ConvertUintToVector4(materialColor);
    material->color = color;
    material->enableLighting = enableLighting;
    material->uvTransform = uvTransform;

    // WVP
    wvp->World = world;
    wvp->WVP = wvpMatrix;

    // テクスチャ
    const TextureData* tex = GetTexture(textureNumber);

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
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(dxManager->GetsrvDescriptorHeap(), descriptorSizeSRV, text.number + 1);
    text.textureSrvHandleGPU = GetGPUDescriptorHandle(dxManager->GetsrvDescriptorHeap(), descriptorSizeSRV, text.number + 1);

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
    // AABB
    obj.aabb = CreateLocalAABB(obj.modelData);
    // 識別ナンバー
    obj.number = static_cast<uint32_t>(objects.size());

    // まず空のObject3Dをvectorに追加し、参照を取得
    objects.push_back(obj);
    Object3D& ref = objects.back();

    // 頂点バッファ作成
    ref.vertexBufferSize = sizeof(VertexData) * UINT(ref.modelData.vertices.size());
    ref.vertexBuffer = CreateBufferResource(dxManager->GetDevice(), ref.vertexBufferSize);
    VertexData* vData = nullptr;
    ref.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vData));
    std::memcpy(vData, ref.modelData.vertices.data(), ref.vertexBufferSize);
    ref.vertexBuffer->Unmap(0, nullptr);

    ref.vertexBufferView.BufferLocation = ref.vertexBuffer->GetGPUVirtualAddress();
    ref.vertexBufferView.SizeInBytes = static_cast<UINT>(ref.vertexBufferSize);
    ref.vertexBufferView.StrideInBytes = sizeof(VertexData);

    return ref.number;
}

// 描画
void Game::Drawobj(const Transforms& transform, const Vector3& center, uint32_t objectNumber, uint32_t textureNumber, const uint32_t& materialColor)
{
    if (objectNumber >= objects.size()) return;

    Object3D& obj = objects[objectNumber];
    const uint32_t kSumVertex = static_cast<uint32_t>(obj.modelData.vertices.size());


    // 1. centerを中心に拡縮・回転
    Matrix4x4 toCenter = Matrix4x4::MakeTranslateMatrix({ -center.x, -center.y, -center.z });
    Matrix4x4 rotateScale = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotate, { 0,0,0 });
    Matrix4x4 fromCenter = Matrix4x4::MakeTranslateMatrix(center);
    Matrix4x4 centerMatrix = (fromCenter * (rotateScale * toCenter));

    // 2. 回転・拡縮後の原点座標を求める
    Vector3 origin = { 0, 0, 0 };
    Vector3 rotatedOrigin = Transform(origin, centerMatrix);

    // 3. translateとの差分を補正移動として加える
    Vector3 offset = {
        transform.translate.x - rotatedOrigin.x,
        transform.translate.y - rotatedOrigin.y,
        transform.translate.z - rotatedOrigin.z
    };
    Matrix4x4 offsetMatrix = Matrix4x4::MakeTranslateMatrix(offset);

    // 4. 最終ワールド行列
    Matrix4x4 worldMatrix = (centerMatrix * offsetMatrix);

    // WVP行列
    Matrix4x4 wvpMatrix = (worldMatrix * cameraController->viewProjectionMatrix);

    wvpData[drawCallIndex]->World = worldMatrix;
    wvpData[drawCallIndex]->WVP = wvpMatrix;

    const TextureData* tex = GetTexture(textureNumber);
    if (!tex) return;

    Vector4 color = ConvertUintToVector4(materialColor);
    //Vector4 rgbaColor = ConvertARGBtoRGBA(color);
    materialData[drawCallIndex]->color = color;
    materialData[drawCallIndex]->enableLighting = true;
    materialData[drawCallIndex]->uvTransform = Matrix4x4::MakeIdentity4x4();

    // 頂点バッファをバインド（描画に使う頂点データを指定）
    dxManager->GetCommandList()->IASetVertexBuffers(0, 1, &obj.vertexBufferView);
    // プリミティブトポロジ（描画する形状の種類：三角形リスト）を設定
    dxManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources[drawCallIndex]->GetGPUVirtualAddress());
    // ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources[drawCallIndex]->GetGPUVirtualAddress());
    // ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
    dxManager->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
    // ルートパラメータ3にディレクショナルライト用定数バッファをバインド
    dxManager->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
    // 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
    dxManager->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);

    drawCallIndex++;
}

void Game::DrawTriangle(const Transforms& localTransform, const Transforms& worldTransform, const VertexData* vertexData, uint32_t textureNumber, const uint32_t& materialColor)
{
    Matrix4x4 world = (
        Matrix4x4::MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate) *
        Matrix4x4::MakeAffineMatrix(worldTransform.scale, worldTransform.rotate, worldTransform.translate)
        );
    Matrix4x4 wvpMatrix = (world * cameraController->viewProjectionMatrix);

    DrawData drawData = SetupDrawData(
        vertexResourceSizeTriangle,
        vertexData,
        3,
        vertexResourceTriangle,
        vertexResourceSizeTriangle,
        materialData[drawCallIndex],
        materialColor,
        true,
        Matrix4x4::MakeIdentity4x4(),
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

void Game::DrawSphere(const Transforms& localTransform, VertexData* vertexData, uint32_t kSubdivision, uint32_t textureNumber, const uint32_t& materialColor)
{
    // 必要な頂点数
    const uint32_t kSumVertex = kSubdivision * kSubdivision * 6;

    // 頂点
    CreateSphere(vertexData, kSubdivision);

    Matrix4x4 world = Matrix4x4::MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate);
    Matrix4x4 wvpMatrix = (world * cameraController->viewProjectionMatrix);

    DrawData drawData = SetupDrawData(
        vertexResourceSizeSphere,
        vertexData,
        kSumVertex,
        vertexResourceSphere,
        vertexResourceSizeSphere,
        materialData[drawCallIndex],
        materialColor,
        true,
        Matrix4x4::MakeIdentity4x4(),
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

void Game::DrawSprite(const Transforms& localTransform, VertexData* vertexData, uint32_t textureNumber, const uint32_t& materialColor)
{
    Matrix4x4 orthoProjectionMatrix = Matrix4x4::MakeOrthographicMatrix(
        0.0f, 0.0f,
        static_cast<float>(windowManager->Getwidth()),
        static_cast<float>(windowManager->Getheight()),
        0.0f, 100.0f);
    Matrix4x4 world = Matrix4x4::MakeAffineMatrix(localTransform.scale, localTransform.rotate, localTransform.translate);
    Matrix4x4 wvpMatrix = (world * orthoProjectionMatrix);

    DrawData drawData = SetupDrawData(
        vertexResourceSizeSprite,
        vertexData,
        4,
        vertexResourceSprite,
        vertexResourceSizeSprite,
        materialData[drawCallIndex],
        materialColor,
        false,
        Matrix4x4::MakeIdentity4x4(),
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

TextureData* Game::GetTexture(uint32_t textureNumber)
{
    for (auto& t : textures)
    {
        if (t.number == textureNumber)
        {
            return &t;
        }
    }
    return nullptr;
}

// 音
uint32_t Game::LoadAudio(const std::string& filePath)
{
    return dxManager->GetAudioManager()->LoadAudio(filePath);
}

void Game::PlayAudio(const uint32_t& audioId, bool loop)
{
    dxManager->GetAudioManager()->PlayAudio(audioId, loop);
}

void Game::StopAudio(const uint32_t& audioId)
{
    dxManager->GetAudioManager()->StopAudio(audioId);
}

void Game::SetAudioVolume(const uint32_t& audioId, float volume)
{
    dxManager->GetAudioManager()->SetVolume(audioId, volume);
}

void Game::SetMasterVolume(float volume)
{
    dxManager->GetAudioManager()->SetMasterVolume(volume);
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

void Game::SetMouseRay()
{
    // hwnd: ゲームウィンドウのハンドル（WindowManagerなどから取得）
    POINT mousePosScreen;
    GetCursorPos(&mousePosScreen); // 画面座標で取得

    // クライアント座標（ウィンドウ左上基準）に変換
    ScreenToClient(windowManager->GetHwnd(), &mousePosScreen);

    // mousePosScreen.x, mousePosScreen.y がウィンドウ内のマウス座標
    mouseController->SetMousePosition({ float(mousePosScreen.x) ,float(mousePosScreen.y) });
    mouseController->SetMouseRay(windowManager->Getwidth(), windowManager->Getheight(), cameraController->viewProjectionMatrix);
}

bool Game::IsCollisionMouseRayAABB(AABB aabb, int objNum)
{
    return IsCollision(mouseController->GetMouseRay(), aabb, objects[objNum].modelData.vertices, Matrix4x4::MakeAffineMatrix(objects[objNum].transform.scale, objects[objNum].transform.rotate, objects[objNum].transform.translate));
};

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
    // ミドルボタン（マウスホイールクリック）
    if (i == 2)
    {
        bool middleButton = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
        return middleButton;
    }

    return false;
}

int Game::GetWheel()
{
    int delta = wheelDelta;
    wheelDelta = 0;
    return delta;
}

// カメラ操作
void Game::MoveCenterTarget(Vector3 target, int spendFrame)
{
    cameraController->SetCenterTarget(target, spendFrame);
}

void Game::MoveRotateTarget(Vector3 target, int spendFrame)
{
    cameraController->SetRotateTarget(target, spendFrame);
}

void Game::MoveDistanceTarget(float target, int spendFrame)
{
    cameraController->SetDistanceTarget(target, spendFrame);
}

AABB Game::CreateAABB(const Transforms& transforms, uint32_t objectNumber)
{
    Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transforms.scale, transforms.rotate, transforms.translate);

    Object3D& obj = objects[objectNumber];

    // ローカルAABBの8頂点
    Vector3 corners[8] = {
        {obj.aabb.min.x, obj.aabb.min.y, obj.aabb.min.z},
        {obj.aabb.max.x, obj.aabb.min.y, obj.aabb.min.z},
        {obj.aabb.min.x, obj.aabb.max.y, obj.aabb.min.z},
        {obj.aabb.max.x, obj.aabb.max.y, obj.aabb.min.z},
        {obj.aabb.min.x, obj.aabb.min.y, obj.aabb.max.z},
        {obj.aabb.max.x, obj.aabb.min.y, obj.aabb.max.z},
        {obj.aabb.min.x, obj.aabb.max.y, obj.aabb.max.z},
        {obj.aabb.max.x, obj.aabb.max.y, obj.aabb.max.z},
    };

    // 8頂点をワールド空間に変換
    Vector3 worldMin = Transform(corners[0], worldMatrix);
    Vector3 worldMax = worldMin;

    for (int i = 1; i < 8; ++i)
    {
        Vector3 v = Transform(corners[i], worldMatrix);
        worldMin.x = my_min(worldMin.x, v.x);
        worldMin.y = my_min(worldMin.y, v.y);
        worldMin.z = my_min(worldMin.z, v.z);
        worldMax.x = my_max(worldMax.x, v.x);
        worldMax.y = my_max(worldMax.y, v.y);
        worldMax.z = my_max(worldMax.z, v.z);
    }
    return { worldMin, worldMax };
}

// int型のcolorをVector4に変換
Vector4 Game::ConvertUintToVector4(uint32_t color)
{
    float r = ((color >> 24) & 0xFF) / 255.0f;
    float g = ((color >> 16) & 0xFF) / 255.0f;
    float b = ((color >> 8) & 0xFF) / 255.0f;
    float a = (color & 0xFF) / 255.0f;
    return { r, g, b, a };
}

Vector4 Game::ConvertARGBtoRGBA(const Vector4& argb)
{
    return { argb.y, argb.z, argb.w, argb.x };
}

AABB Game::CreateLocalAABB(const ModelData& model)
{
    AABB localAABB;

    // 最小値と最大値を初期化
    // 浮動小数点数の最大値で初期化することで、最初の頂点で確実に更新されるようにします
    localAABB.min.x = (std::numeric_limits<float>::max)();
    localAABB.min.y = (std::numeric_limits<float>::max)();
    localAABB.min.z = (std::numeric_limits<float>::max)();

    // 浮動小数点数の最小値で初期化することで、最初の頂点で確実に更新されるようにします
    localAABB.max.x = std::numeric_limits<float>::lowest(); // または -std::numeric_limits<float>::max()

    // モデルの頂点が一つも無い場合（エラーハンドリング）
    if (model.vertices.empty())
    {
        // デフォルト値やエラーを返すなど、適切な処理を行う
        // ここでは便宜上、中心0、サイズ0のAABBを返す
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
