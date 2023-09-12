#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class VisualsMenu : public UIComponent
	{
	public:
		void Render() final;
		void Release() final;
		
	private:
		struct Visuals
		{
			//DOF
			bool* dofActive;
			float* dofFarBlur;
			float* dofFarStart;
			float* dofFarEnd;

			float* dofNearBlur;
			float* dofNearStart;
			float* dofNearEnd;

			float* dofBias;

			//SUN
			float* sunColor;
			float* sunPosition;
		};
		void Initialize() final;
		Visuals visuals;

	};
}