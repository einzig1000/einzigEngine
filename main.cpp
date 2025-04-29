#include <Windows.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <string>
#include <sstream> // std::ostringstream を使用するために必要
#include <format>
#include <strsafe.h>
#include <d3d12.h>
#include <vector>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxcapi.h>
#pragma comment (lib, "dxcompiler")
#include <dxgidebug.h>
#pragma comment (lib, "dxguid.lib")
#include <DbgHelp.h>
#pragma comment (lib, "Dbghelp.lib")
#include <cassert>

#include "functions.h"
#include "definition.h"


// 文字列変換
std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

// 文字列変換
std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

// D3D12_RESOURCE_STATES を文字列に変換する関数
std::string ResourceStateToString(D3D12_RESOURCE_STATES state) {
	switch (state) {
	case D3D12_RESOURCE_STATE_COMMON: return "COMMON";
	case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER: return "VERTEX_AND_CONSTANT_BUFFER";
	case D3D12_RESOURCE_STATE_INDEX_BUFFER: return "INDEX_BUFFER";
	case D3D12_RESOURCE_STATE_RENDER_TARGET: return "RENDER_TARGET";
	case D3D12_RESOURCE_STATE_UNORDERED_ACCESS: return "UNORDERED_ACCESS";
	case D3D12_RESOURCE_STATE_DEPTH_WRITE: return "DEPTH_WRITE";
	case D3D12_RESOURCE_STATE_DEPTH_READ: return "DEPTH_READ";
	case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE: return "NON_PIXEL_SHADER_RESOURCE";
	case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE: return "PIXEL_SHADER_RESOURCE";
	case D3D12_RESOURCE_STATE_STREAM_OUT: return "STREAM_OUT";
	case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT: return "INDIRECT_ARGUMENT";
	case D3D12_RESOURCE_STATE_COPY_DEST: return "COPY_DEST";
	case D3D12_RESOURCE_STATE_COPY_SOURCE: return "COPY_SOURCE";
	case D3D12_RESOURCE_STATE_RESOLVE_DEST: return "RESOLVE_DEST";
	case D3D12_RESOURCE_STATE_RESOLVE_SOURCE: return "RESOLVE_SOURCE";
	case D3D12_RESOURCE_STATE_VIDEO_DECODE_READ: return "VIDEO_DECODE_READ";
	case D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE: return "VIDEO_DECODE_WRITE";
	case D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ: return "VIDEO_PROCESS_READ";
	case D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE: return "VIDEO_PROCESS_WRITE";
	case D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ: return "VIDEO_ENCODE_READ";
	case D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE: return "VIDEO_ENCODE_WRITE";
	default: return "UNKNOWN_STATE";
	}
}

// ウィンドウプロシージャ(クリックした、×を押した等のイベントを処理する関数)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// Imgui用
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		// ウィンドウが破壊された
	case WM_DESTROY:
		// OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ログを出す関数
void Log(const std::string& message)
{
	// string型からchar*型に変換した文字列
	OutputDebugStringA(message.c_str());
}
// Vector4型用のオーバーロード
void Log(const std::string& message, const Vector4& vector)
{
	Log(message);
	std::string a = std::format("x={}, y={}, z={}, w={}", vector.x, vector.y, vector.z, vector.w);
	Log(a);
}
// Matrix4x4型用のオーバーロード
void Log(const std::string& message, const Matrix4x4& matrix)
{
	Log(message);
	std::string a = ":\n";
	for (int i = 0; i < 4; ++i) {
		a += std::format("[{}, {}, {}, {}]\n", matrix.m[i][0], matrix.m[i][1], matrix.m[i][2], matrix.m[i][3]);
	}
	Log(a);
}
// barrier.Transitionの状態をログに出力する関数
void Log(const std::string& message, const D3D12_RESOURCE_BARRIER& barrier)
{
	Log(message);
	if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
		std::string stateBefore = ResourceStateToString(barrier.Transition.StateBefore);
		std::string stateAfter = ResourceStateToString(barrier.Transition.StateAfter);
		std::string a = std::format("Barrier Transition - StateBefore: {}, StateAfter: {}", stateBefore, stateAfter);
		Log(a);
	}
	else {
		Log("Barrier is not of type TRANSITION.");
	}
}
// RootSignatureの状態をログに出力する関数
void Log(const D3D12_ROOT_SIGNATURE_DESC& desc)
{
	std::ostringstream oss;
	oss << "[Root Signature]\n"
		<< "NumParameters: " << desc.NumParameters << "\n"
		<< "NumStaticSamplers: " << desc.NumStaticSamplers << "\n"
		<< "Flags: " << desc.Flags << "\n";

	Log(oss.str());

	for (UINT i = 0; i < desc.NumParameters; ++i)
	{
		const auto& param = desc.pParameters[i];
		oss.str(""); // バッファをクリア
		oss.clear(); // 状態をリセット
		oss << "[Root Parameter " << i << "]\n"
			<< "Type: " << param.ParameterType << "\n"
			<< "ShaderVisibility: " << param.ShaderVisibility << "\n";
		Log(oss.str());

		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV ||
			param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV ||
			param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
		{
			oss.str("");
			oss.clear();
			oss << "ShaderRegister: " << param.Descriptor.ShaderRegister << "\n"
				<< "RegisterSpace: " << param.Descriptor.RegisterSpace << "\n";
			Log(oss.str());
		}
		else if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			oss.str("");
			oss.clear();
			oss << "NumDescriptorRanges: " << param.DescriptorTable.NumDescriptorRanges << "\n";
			Log(oss.str());
			for (UINT j = 0; j < param.DescriptorTable.NumDescriptorRanges; ++j)
			{
				const auto& range = param.DescriptorTable.pDescriptorRanges[j];
				oss.str("");
				oss.clear();
				oss << "  [Descriptor Range " << j << "]\n"
					<< "  RangeType: " << range.RangeType << "\n"
					<< "  BaseShaderRegister: " << range.BaseShaderRegister << "\n"
					<< "  NumDescriptors: " << range.NumDescriptors << "\n"
					<< "  RegisterSpace: " << range.RegisterSpace << "\n";
				Log(oss.str());
			}
		}
	}

	for (UINT i = 0; i < desc.NumStaticSamplers; ++i)
	{
		const auto& sampler = desc.pStaticSamplers[i];
		oss.str("");
		oss.clear();
		oss << "[Static Sampler " << i << "]\n"
			<< "ShaderRegister: " << sampler.ShaderRegister << "\n"
			<< "Filter: " << sampler.Filter << "\n"
			<< "AddressU: " << sampler.AddressU << "\n"
			<< "AddressV: " << sampler.AddressV << "\n"
			<< "AddressW: " << sampler.AddressW << "\n"
			<< "ShaderVisibility: " << sampler.ShaderVisibility << "\n";
		Log(oss.str());
	}
}

// ログをファイルに書き出す
void Log(std::ofstream& os, const std::string& message)
{
	os << message << std::endl;
	OutputDebugStringA(message.c_str());
}

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception)
{
	//時刻を取得して、時刻を名前に入れたファイルを作成。Dumpsディレクトリ以下に出力
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	// processId (このexeのId) とクラッシュ (例外)の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	// 設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	// Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	// 他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	// ID3D12Resourceを格納するポインタ
	ID3D12Resource* pResource = nullptr;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	// リソース記述子を作成
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes; // Vector4を３頂点分
	// バッファの場合はこれらは１にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// リソースを作成
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,        // ヒープのプロパティ
		D3D12_HEAP_FLAG_NONE,   // ヒープフラグ
		&resourceDesc,          // リソースの記述子
		D3D12_RESOURCE_STATE_GENERIC_READ,           // 初期状態
		nullptr,                // Clear値 (バッファの場合はnullptr)
		IID_PPV_ARGS(&pResource) // ID3D12Resourceポインタを取得
	);

	assert(SUCCEEDED(hr));

	return pResource; // 作成したリソースを返す
};

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	// ディスクリプタヒープの生成
	ID3D12DescriptorHeap* DescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
	// レンダ―ターゲットビュー用
	DescriptorHeapDesc.Type = heapType;
	// ダブルバッファ用に２つ。多くたってかまわない。
	DescriptorHeapDesc.NumDescriptors = numDescriptors;
	// 
	DescriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	// エラーチェック
	HRESULT hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&DescriptorHeap));
	// ディスクリプタヒープの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
	return DescriptorHeap;
}

// DXCを使ってShaderをCompileする関数
IDxcBlob* CompileShader(
	// CompileするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに仕様するProfile
	const wchar_t* profile,
	// 初期化で生成したものを３つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	///////////////////////////////////////
	//// 1 hlslファイルを読む
	///////////////////////////////////////
	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら停止する
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	///////////////////////////////////////
	//// 2 Compileする
	///////////////////////////////////////
	LPCWSTR arguments[] = {
		filePath.c_str(),			// コンパイル対象のhlslファイル名
		L"-E", L"main",				// エントリーポイントの指定。基本的にmain
		L"-T", profile,				// ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	// デバック用の情報を埋め込む
		L"-Od",						// 最適化を外しておく
		L"-Zpr",					// 目盛レイアウトは行優先
	};
	// 実際にシェーダーをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,	// 読み込んだファイル
		arguments,				// コンパイルオプション
		_countof(arguments),	// コンパイルオプションの数
		includeHandler,			// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)// コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動出来ないなど致命的な状況
	assert(SUCCEEDED(hr));

	///////////////////////////////////////
	//// 3 警告・エラーが出ていないか確認する
	///////////////////////////////////////
	// 警告・エラーが出たらログにだして止める
	IDxcBlobUtf8* shaderError = nullptr;
	IDxcBlobUtf16* outputName = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), &outputName);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());
		assert(false); // コンパイルエラーが発生した場合は停止
	}

	///////////////////////////////////////
	//// 4 Compile結果を受け取って返す
	///////////////////////////////////////
	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded. path:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}

// 1,Textureデータを読む
DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
	// テクスチャファイルを読んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathw = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathw.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミップマップ付きのデータを返す
	return mipImages;
}

// 2,
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	// 1,metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format; // TextureのFormat
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使ってるのは２次元

	// 2,利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// 3,Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, // 初回のResourceState.Textureは基本読むだけ
		nullptr, // Clear最適解。使わないのでnullptr
		IID_PPV_ARGS(&resource) // 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}

// 3,TextureResourceにデータを転送する
[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	// Tetureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READ ResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

// DepthStencilTextureを作る
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1; // mipmapの数
	resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // TextureのFormat
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // ２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使うよーとういう通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(１番遠い状態)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceろあわせる

	// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	return resource;
}




// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	///////////////////////////////////////
	///	初期化
	///////////////////////////////////////
#pragma region

	///////////////////////////////////////
	/// COMの初期化
	///////////////////////////////////////
#pragma region
	CoInitializeEx(0, COINIT_MULTITHREADED);

#pragma endregion

	SetUnhandledExceptionFilter(ExportDump);
	///////////////////////////////////////
	/// ウィンドウクラスを登録する
	///////////////////////////////////////
#pragma region
	// ウィンドウクラス作成
	WNDCLASS wc{};
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// ウィンドウクラスを登録する
	RegisterClass(&wc);
#pragma endregion

	///////////////////////////////////////
	///	ウィンドウサイズを決める
	///////////////////////////////////////
#pragma region
	// クライアント領域のサイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };
	// クライアント領域を元に実際のサイズのwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
#pragma endregion

	///////////////////////////////////////
	///	ウィンドウを生成　CreateWindow
	///////////////////////////////////////
#pragma region
	// ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,		// 利用するウィンドウクラス名
		L"CG2",					// タイトルバーに表示する文字
		WS_OVERLAPPEDWINDOW,	// ウィンドウスタイルの選択
		CW_USEDEFAULT,			// 表示X座標
		CW_USEDEFAULT,			// 表示Y座標
		wrc.right - wrc.left,	// ウィンドウ横幅
		wrc.bottom - wrc.top,	// ウィンドウ縦幅
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		wc.hInstance,			// インスタンスハンドル
		nullptr					// オプション
	);
#pragma endregion

	///////////////////////////////////////
	///	DebugLayer
	///////////////////////////////////////
#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにＧＰＵ側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	///////////////////////////////////////
	///	ウィンドウを表示
	///////////////////////////////////////
#pragma region

	// ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);
	// 出力ウィンドウへの文字出力
	//OutputDebugStringA("Hello,DirectX!\n");
#pragma endregion

	///////////////////////////////////////
	///	現在時刻でログファイルを生成する
	///////////////////////////////////////
#pragma region
	// ログのディレクトリを用意
	std::filesystem::create_directory("logs");
	// 現在時刻を取得(UTF時刻)
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	// ログファイルの名前にコンマ何秒はいらないので削って秒にする
	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
		nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	// 日本時間（PCの設定時間）に変更
	std::chrono::zoned_time localTime{ std::chrono::current_zone(),nowSeconds };
	// formatを使って年月日_時分秒の文字列に変換
	std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	// 時刻を使ってファイル名を決定
	std::string logFilePath = std::string("logs/") + dateString + ".log";
	// ファイルを作って書き込み準備
	std::ofstream logStream(logFilePath);
#pragma endregion

	///////////////////////////////////////
	///	DXGIFactoryの生成
	///////////////////////////////////////
#pragma region
	// DXGIFactoryの生成
	IDXGIFactory7* dxgiFactory = nullptr;
	// HRESULTはWINDOWS系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	// どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));
#pragma endregion

	///////////////////////////////////////
	///	使用するアダプタ(GPU)を決定する
	///////////////////////////////////////
#pragma region
	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;
	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i)
	{
		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); //取得できないのは一大事
		// ソフトウェアアダプタでなければ採用!
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//採用したアダプタの情報をログに出力。
			Log(ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
			Log(logStream, ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr; // ソフトウェアアダプタの場合は見なかったことにする
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);
#pragma endregion

	///////////////////////////////////////
	///	D3D12Deviceの生成
	///////////////////////////////////////
#pragma region
	ID3D12Device* device = nullptr;
	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr))
		{
			// 生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel: {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n"); // 初期化完了のログをだす

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		// 致命的なエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[]{
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用パグによるエラーメッセージ
			// https://stackoverflow.com/questions/69885245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE

		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		// 解放
		infoQueue->Release();

	}


#endif // DEBUG

#pragma endregion

	///////////////////////////////////////
	///	CommandQueueの生成
	///////////////////////////////////////
#pragma region
	// コマンドキューを生成する
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // 必須設定
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//// コマンドキューを生成する
	//ID3D12CommandQueue* commandQueue = nullptr;
	//D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // DIRECTタイプを指定
	//commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // 通常の優先度
	//commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // 特別なフラグなし
	//commandQueueDesc.NodeMask = 0; // 単一アダプタを使用


#pragma endregion

	///////////////////////////////////////
	///	CommandListの生成
	///////////////////////////////////////
#pragma region

	// コマンドアロケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr)); // 生成が失敗した場合は停止

	// コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr)); // 生成が失敗した場合は停止

	// コマンドリストは初期状態でオープンしているため、必要に応じてCloseする
	//hr = commandList->Close();
	//assert(SUCCEEDED(hr));


#pragma endregion

	///////////////////////////////////////
	///	SwapChainの生成
	///////////////////////////////////////
#pragma region
	// スワップチェーンを生成する
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Width = kClientWidth;
	// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = kClientHeight;
	// 色の形式
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// マルチサンプルしない
	swapChainDesc.SampleDesc.Count = 1;
	// 描画のターゲットとして利用する
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// ダブルバッファ
	swapChainDesc.BufferCount = 2;
	// モニタにうつしたら、中身を破棄
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));

#pragma endregion

	///////////////////////////////////////
	///	SwapChainからResourceを引っ張ってくる
	///////////////////////////////////////
#pragma region
	// SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources[2] = { nullptr };
	// Resourceの取得がうまくいかなかったので起動できない
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));

#pragma endregion

	///////////////////////////////////////
	/// ここで三角形の画像を設定してる
	///	画像用のResourseを作成しデータを書き込む
	///////////////////////////////////////
#pragma region
	DirectX::ScratchImage mipImage = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImage.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
	ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImage, device, commandList);
	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);

#pragma endregion

	///////////////////////////////////////
	/// ここで三角形の色を設定してる
	///	Material用のResourseを作成しデータを書き込む
	///////////////////////////////////////
#pragma region
	// マテリアルリソース？を作る
	ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Vector4));
	// マテリアルにデータを書き込む
	Vector4* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 今回は赤を書き込んでみる
	//*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData[0] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData[1] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	// ログ
	Log("Material Color: [{}]", *materialData);
	materialResource->Unmap(0, nullptr);
#pragma endregion

	///////////////////////////////////////
	/// ここで三角形の位置を設定してる
	///	頂点用のResourseを作成しデータを書き込む
	///////////////////////////////////////
#pragma region
	// 頂点リソースを作る
	ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * 6);
	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	// 上
	vertexData[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	// 右下
	vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };
	
	// 左下2
	vertexData[3].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	vertexData[3].texcoord = { 0.0f,1.0f };
	// 上2
	vertexData[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexData[4].texcoord = { 0.5f,0.0f };
	// 右下2
	vertexData[5].position = { 0.5f, -0.5f, -0.5f, 1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };

#pragma endregion

	///////////////////////////////////////
	/// ここでスプライトの三角形の位置を設定してる
	/// Sprite用 の VertexResourceSprite と VertexBufferViewSprite 作成
	///////////////////////////////////////
#pragma region
	// Sprite用の頂点リソースを作る
	ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);
	// 頂点リソースにデータを書き込む
	VertexData* vertexDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	// 左下
	vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	// 上
	vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	// 右下
	vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };

	// 左下2
	vertexDataSprite[3].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[3].texcoord = { 0.0f,0.0f };
	// 上2
	vertexDataSprite[4].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[4].texcoord = { 1.0f,0.0f };
	// 右下2
	vertexDataSprite[5].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[5].texcoord = { 1.0f,1.0f };

#pragma endregion

	///////////////////////////////////////
	///	DescriptorHeapの生成
	///////////////////////////////////////
#pragma region
	// Render Target View
	ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// Shader Resource View
	ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// Depth Stencil View
	ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

#pragma endregion

	///////////////////////////////////////
	///	VertexBufferViewを生成する
	///////////////////////////////////////
#pragma region
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 仕様するリソースのサイズは頂点３つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	// １頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
#pragma endregion

	///////////////////////////////////////
	///	Sprite用 の VertexBufferView を生成する
	///////////////////////////////////////
#pragma region
	// Sprite頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 仕様するリソースのサイズは頂点３つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	// １頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

#pragma endregion

	///////////////////////////////////////
	///	RenderTargetViewを作る
	///////////////////////////////////////
#pragma region
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	// 出力結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 2dテクスチャとして書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	// ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// RTVを２つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	// まず１つ目を作る。１つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	// ２つ目のディスクリプタハンドルを得る（自力？で）
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// ２つ目を作る
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

#pragma endregion

	///////////////////////////////////////
	/// ShaderResourceViewを作る
	///////////////////////////////////////
#pragma region
	// metaDataを基にSRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	// 先頭はImGuiが使ってるのでその次を使う
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRVの作成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);

#pragma endregion

	///////////////////////////////////////
	///	DepthStencilViewを作る
	///////////////////////////////////////
#pragma region
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	// DSVHeapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());



#pragma endregion

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	///////////////////////////////////////
	///	コマンドを積み込んで確定させる　&&　	TransitionBarrierを張る 
	///////////////////////////////////////
#pragma region


	///////////////////////////////////////
	///	TransitionBarrierを張る
	///////////////////////////////////////
#pragma region
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバッファに対して行う
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	// 遷移前（現在）のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);
	Log("Barrier State", barrier);
#pragma endregion

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// 描画先のRTVを設定する
	//commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };// 青っぽい色。RGBAの順
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);




	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	///////////////////////////////////////
	///	ImGuiでも利用する描画用のdescriptorHeapの設定
	///////////////////////////////////////

	// ImGui 描画前にディスクリプタヒープを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);


	///////////////////////////////////////
	/// 画面表示できるようにする
	///////////////////////////////////////
#pragma region
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList->ResourceBarrier(1, &barrier);


#pragma endregion

	// コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
	hr = commandList->Close();
	assert(SUCCEEDED(hr));

#pragma endregion

	///////////////////////////////////////
	/// メインループの開始前に作る
	///	FenceとEventを生成する
	///////////////////////////////////////
#pragma region
	// 初期値０でFenceを作る
	ID3D12Fence* fence = nullptr;
	uint64_t FenceValue = 0;
	hr = device->CreateFence(FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	// FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

#pragma endregion

	///////////////////////////////////////
	/// fenceEventを作成した直後あたりがよい
	///	DXCの初期化
	///////////////////////////////////////
#pragma region

	// dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

#pragma endregion

	///////////////////////////////////////
	///	コマンドをキックする
	///////////////////////////////////////
#pragma region
	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);
	// GPUとOSに画面の交換を行うよう通知する
	swapChain->Present(1, 0);

	///////////////////////////////////////
	///	GPUにSignalを送る
	///////////////////////////////////////
	// Fenceの値を更新
	FenceValue++;
	// GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue->Signal(fence, FenceValue);

	///////////////////////////////////////
	///	Feenceの値を確認してGPUを待つ
	///////////////////////////////////////
#pragma region
	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence->GetCompletedValue() < FenceValue)
	{
		// 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
		fence->SetEventOnCompletion(FenceValue, fenceEvent);
		// イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}

#pragma endregion


	// 次のフレーム用のコマンドリスを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));

#pragma endregion

	///////////////////////////////////////
	///	PSO
	///////////////////////////////////////
#pragma region

	///////////////////////////////////////
	/// DescriptorRangeの作成
	///////////////////////////////////////
#pragma region
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // t0 に対応
	descriptorRange[0].NumDescriptors = 1; // t0のみ使う　2だったらt0とt1の２つ使う
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion

	///////////////////////////////////////
	///	RootSignatureの作成
	///////////////////////////////////////
#pragma region

	// RootParametersの設定
	D3D12_ROOT_PARAMETER rootParameters[3] = {};

	// b0: 定数バッファビュー (CBV)
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// s0: 
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	// t0: シェーダーリソースビュー (SRV)
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

#pragma endregion

	///////////////////////////////////////
	///	Samplerの作成
	///////////////////////////////////////
#pragma region
	// 静的サンプラーの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0; // s0 に対応
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// ルートシグネチャの作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元にルートシグネチャを生成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	// ログ
	Log(descriptionRootSignature);

#pragma endregion

	///////////////////////////////////////
	///	InputLayoutの設定を行う
	///////////////////////////////////////
#pragma region
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


#pragma endregion

	///////////////////////////////////////
	///	BlendStateの設定を行う
	///////////////////////////////////////
#pragma region
	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// 全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
#pragma endregion

	///////////////////////////////////////
	///	RasterizerStateの設定を行う
	///////////////////////////////////////
#pragma region
	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
#pragma endregion

	///////////////////////////////////////
	///	シェーダー(VertexShader&PixelShader)をコンパイルする
	///////////////////////////////////////
#pragma region
	// Pixel Shader のコンパイル
	IDxcBlob* pixelShaderBlob = CompileShader(L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	// Vertex Shader のコンパイル
	IDxcBlob* vertexShaderBlob = CompileShader(L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

#pragma endregion

	///////////////////////////////////////
	///	DepthStencilStateの設定を行う
	///////////////////////////////////////
#pragma region
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込み？
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessWqual。つまり、近ければ描画される。
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

	///////////////////////////////////////
	///	PSOを生成する
	///////////////////////////////////////
#pragma region
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	// RootSignature
	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	// InputLayout
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	// BlendState
	graphicsPipelineStateDesc.BlendState = blendDesc;
	// RasterizerState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// VertexShader
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	// PixelShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	// DepthStencilState
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//書き込むRTV情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma endregion

	///////////////////////////////////////
	///	TransformationMatrix用のResourceを作る & CBVを設定する
	///////////////////////////////////////
#pragma region
	// World-View-Projection用のリソースを作る。Matrix4x4　１つ分のサイズを用意する
	ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(Matrix4x4));
	// データを書き込む
	Matrix4x4* wvpData = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込んでおく
	*wvpData = MakeIdentity4x4();
	// ログを出したい気分
	Log("WVP Matrix", *wvpData);
	wvpResource->Unmap(0, nullptr);


	// Sprite用のWorld-View-Projection用のリソースを作る。Matrix4x4　１つ分のサイズを用意する
	ID3D12Resource* TransformationMatrixResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));;
	// データを書き込む
	Matrix4x4* TransformationMatrixDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	TransformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&TransformationMatrixDataSprite));
	// 単位行列を書き込んでおく
	*TransformationMatrixDataSprite = MakeIdentity4x4();


#pragma endregion

	///////////////////////////////////////
	///	ViewportとScissor
	///////////////////////////////////////
#pragma region
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;

#pragma endregion

	///////////////////////////////////////
	/// imguiの初期化
	///////////////////////////////////////
#pragma region
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);
#pragma endregion

	///////////////////////////////////////
	///	scale,rotate,translateを作成
	///////////////////////////////////////
#pragma region
	// 三角形のSRT
	Transforms transform{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	// Sprite用の三角形のSRT
	Transforms transformSprite{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	// カメラのSRT
	Transforms cameraTransform{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,-5.0f}, {0.0f,0.0f,0.0f} };



#pragma endregion

	///////////////////////////////////////
	///	ImGui用に追加変数
	///////////////////////////////////////
#pragma region

	const char* items[] = { "Red", "Green", "Blue" };
	static int item_current = 0;


#pragma endregion




#pragma endregion

	///////////////////////////////////////
	///	メインループ
	///////////////////////////////////////
	MSG msg{};
	// ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT)
	{
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// ゲームの処理
			///////////////////////////////////////
			///	ImGuiを使う ここからフレームが始まるぜとimguiに伝える
			///////////////////////////////////////
#pragma region

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGui::ShowDemoWindow(); 

			ImGui::Begin("CG2_02");
			ImGui::Combo("Combo Box", &item_current, items, IM_ARRAYSIZE(items));
			if (item_current == 0) {};
			if (item_current == 1) {};
			if (item_current == 2) {};

			ImGui::Text("camera");
			ImGui::DragFloat3("cameraPosition", &cameraTransform.translate.x, 0.01f);
			ImGui::DragFloat3("cameraRotate", &cameraTransform.rotate.x, 0.01f);
			ImGui::Text("3d");
			ImGui::DragFloat2("Position", &transform.translate.x, 0.01f);
			ImGui::ColorEdit3("Pick a color", (float*)&materialData[0].x);
			ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
			ImGui::Text("sprite");
			ImGui::DragFloat2("SpritePosition", &transformSprite.translate.x, 1.0f);
			ImGui::End();



#pragma endregion

			///////////////////////////////////////
			///	TransitionBarrierを張る(TransitionBarrierの命令を実行する)
			///////////////////////////////////////
#pragma region
			// これから書き込むバックバッファのインデックスを取得
			backBufferIndex = swapChain->GetCurrentBackBufferIndex();

			// バリアを張る対象のリソース。現在のバッファに対して行う
			barrier.Transition.pResource = swapChainResources[backBufferIndex];
			// 遷移前（現在）のResourceState
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			// 遷移後のResourceState
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			// TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);

			//描画先のRTVを設定する
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
			//指定した色で画面全体をクリアする
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
#pragma endregion

			transform.rotate.y += 0.00f;

			///////////////////////////////////////
			///	TransFormを使ってCBufferを更新する
			///////////////////////////////////////
#pragma region

			// 三角形用のWorldViewProjectionMatrixを作る
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
			// WVPMatrixを作る
			Matrix4x4 worldViewProjectionMatrix = Mul(worldMatrix, Mul(viewMatrix, projectionMatrix));

			*wvpData = worldViewProjectionMatrix;


			// Sprite用のWorldViewProjectionMatrixを作る
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
			// WVPMatrixを作る
			Matrix4x4 worldViewProjectionMatrixSprite = Mul(worldMatrixSprite, Mul(viewMatrixSprite, projectionMatrixSprite));

			*TransformationMatrixDataSprite = worldViewProjectionMatrixSprite;


#pragma endregion

			// ゲームの処理が終わり描画処理に入る前に、ImGuiの内部コマンドを生成する
			ImGui::Render();

			///////////////////////////////////////
			///	コマンドを積む
			///////////////////////////////////////
#pragma region
			/////
			//	3D三角形の描画 
			/////
#pragma region
			
			// 描画前にディスクリプタヒープを設定
			ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
			commandList->SetDescriptorHeaps(1, descriptorHeaps);
			// Viewportを設定
			commandList->RSSetViewports(1, &viewport);
			// Scirssorを設定
			commandList->RSSetScissorRects(1, &scissorRect);
			// RootSignatureを設定。
			commandList->SetGraphicsRootSignature(rootSignature);
			commandList->SetPipelineState(graphicsPipelineState);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			// 形状を設定
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			// CBVを設定する マテリアル用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			// CBVを設定する wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

			// 描画先のRTVとDSVを設定する
			dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
			// 指定した深度で画面全体をクリアする
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			// 描画
			//commandList->DrawInstanced(6, 1, 0, 0);
			commandList->DrawInstanced(6, 1, 0, 0);
#pragma endregion

			/////
			//	Spriteの描画 
			/////
#pragma region
			
			// Spriteの描画
			commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			commandList->SetGraphicsRootConstantBufferView(1, TransformationMatrixResourceSprite->GetGPUVirtualAddress());

			// 描画
			commandList->DrawInstanced(6, 1, 0, 0);
#pragma endregion

#pragma endregion

			///////////////////////////////////////
			///	ImGuiを描画する
			///////////////////////////////////////
#pragma region

			// 実際のcommandListのImGuiの描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

#pragma endregion

			///////////////////////////////////////
			/// ResourceStateを入れ替える
			///////////////////////////////////////
#pragma region

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			commandList->ResourceBarrier(1, &barrier);

#pragma endregion

			///////////////////////////////////////
			///	コマンドリストを確定させる
			///////////////////////////////////////
#pragma region
			// コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
			hr = commandList->Close();
			if (FAILED(hr)) {
				Log("Failed to close command list.");
				assert(false);
			}

			//assert(SUCCEEDED(hr));
#pragma endregion

			///////////////////////////////////////
			///	コマンドをキックする
			///////////////////////////////////////
#pragma region
			// GPUにコマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { commandList };
			commandQueue->ExecuteCommandLists(1, commandLists);
			// GPUとOSに画面の交換を行うよう通知する
			swapChain->Present(1, 0);
#pragma endregion

			///////////////////////////////////////
			///	キックしおわったらGPUにSignalを送る
			///////////////////////////////////////
#pragma region
			// Fenceの値を更新
			FenceValue++;
			// GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
			commandQueue->Signal(fence, FenceValue);

#pragma endregion

			///////////////////////////////////////
			///	Fenceの値を確認してGPUを待つ
			///////////////////////////////////////
#pragma region
			// Fenceの値が指定したSignal値にたどり着いているか確認する
			// GetCompletedValueの初期値はFence作成時に渡した初期値
			if (fence->GetCompletedValue() < FenceValue)
			{
				// 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
				fence->SetEventOnCompletion(FenceValue, fenceEvent);
				// イベント待つ
				WaitForSingleObject(fenceEvent, INFINITE);
			}
#pragma endregion

			///////////////////////////////////////
			/// 次のフレーム用のコマンドリストを準備
			///////////////////////////////////////
#pragma region
			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator, nullptr);
			assert(SUCCEEDED(hr));
#pragma endregion



		}
	}

	///////////////////////////////////////
	/// ImGuiの終了処理
	///////////////////////////////////////
#pragma region
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#pragma endregion

	///////////////////////////////////////
	///	COMの終了処理
	///////////////////////////////////////
#pragma region
	CoUninitialize();

#pragma endregion

	///////////////////////////////////////
	///	解放処理
	///////////////////////////////////////
#pragma region
	materialResource->Release();
	wvpResource->Release();
	TransformationMatrixResourceSprite->Release();
	vertexResource->Release();
	vertexResourceSprite->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	textureResource->Release();
	intermediateResource->Release();
	if (errorBlob)
	{
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();

	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	srvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(hwnd);



	// リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

#pragma endregion



	return 0;
}

