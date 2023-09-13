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
			float* sunColorUI;
			float sunPositionXUI;
			float sunPositionYUI;
			float sunPositionZUI;
			float sunPitch;
			float sunYaw;
			float sunBrightness = 1;
		};
		enum Pos
		{
			x, y, z
		};
		void Initialize() final;
		void RenderDOF();
		void RenderSun();
		void UpdateSunColor();
		void UpdateSunPosition(int);
		void UpdateSunAngle();
		Visuals visuals;
		float* sunColor = (float*)0x4;
		float* sunPosition = (float*)0x10;
		float initColors[3] = { 0.6, 0.2, 0.8 };
		float initPosition[3] = { 0, 0, 0 };
	};
}