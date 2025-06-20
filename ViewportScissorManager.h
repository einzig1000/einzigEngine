#pragma once
#include <d3d12.h>

class ViewportScissorManager
{
public:
    ViewportScissorManager(int width, int height);
    ~ViewportScissorManager();

    const D3D12_VIEWPORT& GetViewport() const { return viewport; }
    const D3D12_RECT& GetScissorRect() const { return scissorRect; }

private:
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissorRect;
};