#pragma once
#include "Resources.hpp"

namespace IWXMVM::UI
{
	class UIImage
	{
	private:
		ImVec2 dimensions;
		ImTextureID texture;
	public:
		ImVec2 GetDimensions() const;
		ImTextureID GetTextureID() const;

		//static UIImage FromFile(std::string path);
		static UIImage FromResource(const uint8_t data[], uint32_t size);
		static void ReleaseResource(const uint8_t data[]);
	};
}