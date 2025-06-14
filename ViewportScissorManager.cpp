#include "ViewportScissorManager.h"

ViewportScissorManager::ViewportScissorManager(int width, int height)
{
    viewport.Width = float(width);
    viewport.Height = float(height);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    scissorRect.left = 0;
    scissorRect.right = width;
    scissorRect.top = 0;
    scissorRect.bottom = height;
}