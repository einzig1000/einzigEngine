#include "FrameCbAllocator.h"
#include "Utilities/functions.h"
#include <cassert>
#include <cstring>

void FrameCbAllocator::Initialize(ID3D12Device* device, size_t capacityBytes, const wchar_t* debugName)
{
	assert(device);
	capacity_ = AlignUp(capacityBytes, kAlignment);

	resource_ = CreateBufferResource(device, capacity_);
	assert(resource_);

	resource_->SetName(debugName);

	mapped_ = nullptr;
	HRESULT hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_));
	assert(SUCCEEDED(hr));
	assert(mapped_);

	offset_ = 0;
}

void FrameCbAllocator::Reset()
{
	offset_ = 0;
}

FrameCbAllocator::Allocation FrameCbAllocator::Allocate(size_t sizeBytes)
{
	assert(mapped_);
	size_t alignedSize = AlignUp(sizeBytes, kAlignment);

	size_t alignedOffset = AlignUp(offset_, kAlignment);
	assert(alignedOffset + alignedSize <= capacity_);

	Allocation a{};
	a.cpu = mapped_ + alignedOffset;
	a.gpu = resource_->GetGPUVirtualAddress() + alignedOffset;
	a.size = alignedSize;

	offset_ = alignedOffset + alignedSize;
	return a;
}