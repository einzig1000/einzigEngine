#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cassert>
#include <cstdint>

class RTVManager
{
	public:
	struct RTVAllocation
	{
		uint32_t index;
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
	};
	RTVManager(ID3D12Device* device);
	~RTVManager();
	ID3D12DescriptorHeap* GetRTVDescriptorHeap() const { return descriptorHeap_.Get(); }
	uint32_t GetDescriptorSizeRTV() const { return descriptorSize_; }
	// 空いてるスロットインデックスを取得しnextIndex_をインクリメント
	uint32_t Allocate();
	// Allocate()で取得したスロットインデックスのCPUハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index) const;
	// RTVの作成とスロットの割り当て
	RTVAllocation CreateRTV(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc);
	RTVAllocation CreateRTVforTexture2D(ID3D12Resource* resource, DXGI_FORMAT format);
};

