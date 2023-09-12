#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class VisualsMenu : public UIComponent
	{
	public:
		void Render() final;
		void RenderDOF();
		void RenderSun();
		void UpdateSun();
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
			float* sunColorUI;
			float* sunPosition;
			float sunBrightness = 1;
		};
		void Initialize() final;
		Visuals visuals;
		float* sunColor = (float*)0x4;
		float initColors[3] = { 0.6, 0.2, 0.8 };
	};
}