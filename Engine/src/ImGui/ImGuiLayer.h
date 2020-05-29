#pragma once

#include "pch.h"

namespace sixengine {

	class ImGuiLayer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(const std::string& name);
		~ImGuiLayer();

		void BeginFrame();
		void EndFrame();

		void Init();
		void Shutdown();
	};

}
