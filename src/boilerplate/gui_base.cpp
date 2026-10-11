#include "pch.h"
#include "gui_base.hpp"

auto SettingsWindowBase::GetPluginName() -> std::string { return "Custom Ball Online"; }
auto SettingsWindowBase::SetImGuiContext(uintptr_t ctx) -> void { ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext *>(ctx)); }

auto PluginWindowBase::GetMenuName() -> std::string { return "CustomBallOnline"; }
auto PluginWindowBase::GetMenuTitle() -> std::string { return menuTitle_; }
auto PluginWindowBase::SetImGuiContext(uintptr_t ctx) -> void { ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext *>(ctx)); }
auto PluginWindowBase::ShouldBlockInput() -> bool { return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard; }
auto PluginWindowBase::IsActiveOverlay() -> bool { return true; }
auto PluginWindowBase::OnOpen() -> void { isWindowOpen_ = true; }
auto PluginWindowBase::OnClose() -> void { isWindowOpen_ = false; }

auto PluginWindowBase::Render() -> void {
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None)) {
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	RenderWindow();

	ImGui::End();

	if (!isWindowOpen_) {
		_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}
