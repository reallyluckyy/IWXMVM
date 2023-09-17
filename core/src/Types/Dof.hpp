#pragma once

namespace IWXMVM::Types
{
	struct DoF
	{
		bool enabled;
		float farBlur;
		float farStart;
		float farEnd;
		float nearBlur;
		float nearStart;
		float nearEnd;
		float bias;
	};
}