#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace Dx12ResourceFactory
{
    /// <summary>
    /// バッファリソースを作成する関数
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成されたバッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

    /// <summary>
    /// 定数バッファリソースを作成する関数(256の倍数になる
    /// </summary>
    /// <param name="device">DirectX 12 デバイス</param>
    /// <param name="sizeInBytes">バッファのサイズ (バイト単位)</param>
    /// <returns>作成された定数バッファリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantBufferResource(
        ID3D12Device* device, size_t sizeInBytes);
}