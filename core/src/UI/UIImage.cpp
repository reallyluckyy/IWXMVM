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

    std::unordered_map<std::string, UIImage> fileImageCache;

	UIImage UIImage::FromFile(const std::string path)
	{
        if (fileImageCache.contains(path))
            return fileImageCache[path];

        UIImage image;

        PDIRECT3DTEXTURE9 texture;
        const HRESULT hresult = D3DXCreateTextureFromFileA(Mod::GetGameInterface()->GetD3D9Device(), path.c_str(), &texture);
        if (hresult != S_OK)
            throw std::exception("failed to load image");

        D3DSURFACE_DESC imageDesc;
        texture->GetLevelDesc(0, &imageDesc);

        image.texture = texture;
        image.dimensions = ImVec2(imageDesc.Width, imageDesc.Height);

        fileImageCache.insert({ path, image });

        return image;
	}

    std::unordered_map<uint8_t*, UIImage> resourceImageCache;

    UIImage UIImage::FromResource(const uint8_t data[], uint32_t size)
    {
        if (resourceImageCache.contains((uint8_t*)data))
            return resourceImageCache[(uint8_t*)data];

        UIImage image;

        PDIRECT3DTEXTURE9 texture;
        const HRESULT hresult = D3DXCreateTextureFromFileInMemory(Mod::GetGameInterface()->GetD3D9Device(), data, size, &texture);
        if (hresult != S_OK)
            throw std::exception("failed to load image");

        D3DSURFACE_DESC imageDesc;
        texture->GetLevelDesc(0, &imageDesc);

        image.texture = texture;
        image.dimensions = ImVec2(imageDesc.Width, imageDesc.Height);

        resourceImageCache.insert({ (uint8_t*)data, image });

        return image;
    }
}
