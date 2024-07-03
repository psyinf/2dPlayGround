#pragma once
#include <gui/GameGuiWidget.hpp>
#include <imgui.h>
#include <components/Stats.hpp>
#include <ranges>
#include <foundation/imgui/Spinner.hpp>

namespace galaxy::gui {

class LoadResourcesWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        // TODO: based on the state of the a loading thread, show a loading screen
        // TODO: pause until done
        ImGui::Begin("Loading Resources",
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar);
        // center the window
        ImGui::SetWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowWidth()) * 0.5f,
                                   (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowHeight()) * 0.5f));

        // center horizontally region

        ImGui::BeginGroup();
        // center text
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Loading Resources...").x) * 0.5f);
        ImGui::Text("Loading Resources...");
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 40) * 0.5f);
        ImGui::LoadingIndicatorCircle("##loading",
                                      40,
                                      ImGui::GetStyleColorVec4(ImGuiCol_Text),
                                      ImGui::GetStyleColorVec4(ImGuiCol_WindowBg),
                                      10,
                                      3);
        ImGui::EndGroup();
        ImGui::End();
    }
};

} // namespace galaxy::gui