#include <unordered_map>
#include <string>
#include <d3d11.h>

class TextureManager {
public:
    TextureManager(ID3D11Device* device);
    ~TextureManager();

    ID3D11ShaderResourceView* LoadTexture(const std::wstring& filePath);
    void ReleaseAll();

private:
    ID3D11Device* m_device;
    std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_textures;
};
