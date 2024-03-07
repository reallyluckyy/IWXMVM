#include "StdInclude.hpp"
#include "UIImage.hpp"

#include "Mod.hpp"

namespace IWXMVM::UI
{
    ImVec2 UIImage::GetDimensions() const
    {
        return dimensions;
    }

    ImTextureID UIImage::GetTextureID() const
    {
        return texture;
    }

    /*
std::unordered_map<std::string, UIImage> fileImageCache;

UIImage UIImage::FromFile(const std::string path)
    {
    if (fileImageCache.contains(path))
        return fileImageCache[path];

    UIImage image;

    PDIRECT3DTEXTURE9 texture;
    const HRESULT hresult = D3DXCreateTextureFromFileA(D3D9::GetDevice(), path.c_str(), &texture);
    if (hresult != S_OK)
        throw std::exception("failed to load image");

    D3DSURFACE_DESC imageDesc;
    texture->GetLevelDesc(0, &imageDesc);

    image.texture = texture;
    image.dimensions = ImVec2(imageDesc.Width, imageDesc.Height);

    fileImageCache.insert({ path, image });

    return image;
    }
*/

    std::unordered_map<uint8_t*, UIImage> resourceImageCache;

    UIImage UIImage::FromResource(const uint8_t data[], uint32_t size)
    {
        if (resourceImageCache.contains((uint8_t*)data))
            return resourceImageCache[(uint8_t*)data];

        UIImage image;

        PDIRECT3DTEXTURE9 texture;
        const HRESULT hresult = D3DXCreateTextureFromFileInMemory(D3D9::GetDevice(), data, size, &texture);
        if (hresult != S_OK)
            throw std::exception("failed to load image");

        D3DSURFACE_DESC imageDesc;
        texture->GetLevelDesc(0, &imageDesc);

        image.texture = texture;
        image.dimensions = ImVec2(static_cast<float>(imageDesc.Width), static_cast<float>(imageDesc.Height));

        resourceImageCache.insert({(uint8_t*)data, image});

        return image;
    }

    void UIImage::ReleaseResource(const uint8_t data[])
    {
        try
        {
            auto search = resourceImageCache.find((uint8_t*)data);

            if (search != resourceImageCache.end())
            {
                PDIRECT3DTEXTURE9 texture = (PDIRECT3DTEXTURE9)search->second.texture;
                texture->Release();

                resourceImageCache.erase(search);
            }
        }
        catch (...)
        {
            // TODO: searching the resource container fails when closing the game
            // potential memory leak because the textures are not released

            // possible solution: move ownership away from global container to class instance of ControlBar
        }
    }
}  // namespace IWXMVM::UI