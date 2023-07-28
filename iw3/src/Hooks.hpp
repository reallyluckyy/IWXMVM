#pragma once

#include <d3d9.h>

namespace IWXMVM::IW3::Hooks
{
	void CG_Draw2D_Hook();

	void Install(void** vTable);
}