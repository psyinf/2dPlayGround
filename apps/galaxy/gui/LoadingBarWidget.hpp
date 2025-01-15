#pragma once
#include <gui/GameGuiWidget.hpp>
#include <imgui.h>
#include <components/Stats.hpp>
#include <ranges>
#include <pgFoundation/imgui/Spinner.hpp>

#include <pgGame/events/SceneManagementEvents.hpp>
#include <pgEngine/gui/ImGuiScopedWrappers.hpp>

namespace galaxy::gui {

class LoadingBarWidget : public galaxy::gui::GameGuiWidget
{
    uint32_t numFramesFinished = 0;

public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        float totalProgress = getGame().getCurrentScene().getSingleton<float&>("resourceLoader.totalProgress");
        if (totalProgress >= 1.0f) { numFramesFinished++; }
        if (numFramesFinished > 200) { return; }

        ImGui::Begin("Loading Resources",
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::SetWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - ImGui::GetWindowHeight()));
        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 20));

        ImGui::SetCursorPosX(10);

        {
            pgf::gui::StyleStack styleStack;
            styleStack.pushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
            // if number of frames finished is > 0 slowly fade out
            if (numFramesFinished > 0)
            {
                float alpha = 1.0f - numFramesFinished / 200.0f;
                // use blending to fade out
                styleStack.pushStyle(ImGuiStyleVar_Alpha, alpha);
            }

            ImGui::ProgressBar(totalProgress, ImVec2{ImGui::GetIO().DisplaySize.x - 20, 40}, "");
            // ImGui::Text("Loading... %.2f%%", totalProgress * 100.0f);
        }

        ImGui::End();
    }
};

} // namespace galaxy::gui