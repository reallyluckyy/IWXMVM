#pragma once

namespace IWXMVM::Types
{
	struct Dvar
	{
		std::string name;
		union Value
		{
			float floating_point;
			uint32_t uint32;
			int32_t int32;
			float vector[4];
			const char* string;
			uint8_t color[4];
		}*value;
	};
}