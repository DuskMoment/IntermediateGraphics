#pragma once
#include "../ew/external/glad.h"
#include <stdio.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


namespace wm
{
	struct ImGuiSetting
	{
		virtual void drawSettings()
		{

		};
	};

	struct GrayScaleSettings : public ImGuiSetting
	{
	public:

		float intensity = 1.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("GrayScale Settings"))
			{

				ImGui::SliderFloat("intesity", &intensity, 1.0f, 10.f);
			
			}

		};
	};

	struct KernelBlurSettings : public ImGuiSetting
	{
	public:
		float intensity = 1.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("KernelBlur Settings"))
			{

				ImGui::SliderFloat("intesity", &intensity, 1.0f, 10.f);

			}

		};
	};
	struct ChromaticAberrationSettings : public ImGuiSetting
	{
	public:

		float offsetRed = 0.009;
		float offsetGreen = 0.006;
		float offsetBlue = -0.006;


		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("ChromaticAberration Settings"))
			{

				ImGui::SliderFloat("Red Offset", &offsetRed, -1.000f, 1.0f);
				ImGui::SliderFloat("Green Offset", &offsetGreen, -1.000f, 1.0f);
				ImGui::SliderFloat("Blue Offset", &offsetBlue, -1.000f, 1.0f);

			}

		};
	};

	struct EdgeDetectionSettings : public ImGuiSetting
	{
	public:

		float intensity = 1.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("EdgeDetection Settings"))
			{
				//can cause a divide by zero
				ImGui::SliderFloat("intesity", &intensity, -5.0f, 1.f);

			}
		};
	};

	struct FogSettings : public ImGuiSetting
	{
	public:
		float stepness = 0.7;
		float offset = 25.0;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("Fog Settings"))
			{
				ImGui::SliderFloat("offset", &offset, 0.0f, 70.0f);
				ImGui::SliderFloat("stepness", &stepness, 0.0f, 1.0f);

			}
		};

	};
	struct BloomSettings : public ImGuiSetting
	{
	public:
		float exposure = 2.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("Bloom Settings"))
			{
				ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.0f);
				
			}
		};

	};

	struct HDRSettings : public ImGuiSetting
	{
	public: 
		float exposure = 2.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("HDR Settings"))
			{
				ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.0f);

			}
		}
	};

	struct VignetteSettings : public ImGuiSetting
	{
	public: 

		float intensity = 1.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("Vignette Settings"))
			{
				ImGui::SliderFloat("intesity", &intensity, 1.0f, 10.f);

			}
		};
	};

	struct LensDistortionSettings: public ImGuiSetting
	{
	public:

		float intensity = 1.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("LensDistortion Settings"))
			{
				ImGui::SliderFloat("intesity", &intensity, 1.0f, 10.f);

			}
		};
	};

	struct FilmGrainSettings : public ImGuiSetting
	{
	public:

		float intensity = 1.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("FilmGrain Settings"))
			{
				ImGui::SliderFloat("intesity", &intensity, 1.0f, 10.f);

			}
		};
	};

	struct BoxBlurSettings : public ImGuiSetting
	{
	public:

		float intensity = 1.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("FilmGrain Settings"))
			{
				ImGui::SliderFloat("intesity", &intensity, 1.0f, 10.f);

			}
		};

	};

	struct SharpenSettings : public ImGuiSetting
	{
	public:

		float intensity = 1.0f;

		virtual void drawSettings() override
		{
			if (ImGui::CollapsingHeader("FilmGrain Settings"))
			{
				ImGui::SliderFloat("intesity", &intensity, 1.0f, 10.f);

			}
		};

	};


	std::vector<ImGuiSetting*> createSettingsList()
	{
		std::vector<ImGuiSetting*> settings;

		//I should be keeping track of the ownership for deleting but I dont care :3
		//needs a dummy for the none settings
		settings.push_back(new ImGuiSetting());
		settings.push_back(new GrayScaleSettings());
		settings.push_back(new ImGuiSetting());
		settings.push_back(new ImGuiSetting());
		settings.push_back(new ChromaticAberrationSettings());
		settings.push_back(new EdgeDetectionSettings());
		settings.push_back(new FogSettings());
		settings.push_back(new BloomSettings());
		settings.push_back(new HDRSettings());
		settings.push_back(new VignetteSettings());
		settings.push_back(new LensDistortionSettings());
		settings.push_back(new FilmGrainSettings());
		settings.push_back(new SharpenSettings());

		return settings;

	}

	enum SettingNames
	{
		NONE,
		GRAY_SCALE,
		KERNEL_BLUR,
		INVERSE,
		CHROMATIC_ABERRATION,
		EDGE_DETECTION,
		FOG,
		BLOOM,
		HDR,
		VIGNETTE,
		LENS_DISTORTION,
		FILM_GRAIN,
		SHARPEN
	};
}