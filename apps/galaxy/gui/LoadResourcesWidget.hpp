#pragma once
#include <gui/GameGuiWidget.hpp>
#include <imgui.h>
#include <components/Stats.hpp>
#include <ranges>
#include <foundation/imgui/Spinner.hpp>

#include <pgGame/events/SceneManagementEvents.hpp>

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
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        // center the window
        ImGui::SetWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowWidth()) * 0.5f,
                                   (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowHeight()) * 0.5f));

        // center horizontally region

        ImGui::BeginGroup();
        // center text
        // limit vertical size to 2/3 of the window
        ImGui::SetNextWindowSizeConstraints(ImVec2(-1.f, -1.f),
                                            ImVec2(ImGui::GetWindowWidth() * 0.66f, ImGui::GetWindowHeight() * 0.8f));

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Loading Resources...").x) * 0.5f);
        ImGui::Text("Loading Resources...");
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 40) * 0.5f);
        ImGui::LoadingIndicatorCircle("##loading",
                                      40,
                                      ImGui::GetStyleColorVec4(ImGuiCol_Text),
                                      ImGui::GetStyleColorVec4(ImGuiCol_WindowBg),
                                      10,
                                      3);
        float totalProgress = getGame().getCurrentScene().getSingleton<float&>("resourceLoader.totalProgress");
        // float currentProgress = getGame().getCurrentScene().getSingleton<float&>("resourceLoader.currentProgress");
        // auto& resourcesProgress =
        //     getGame().getCurrentScene().getSingleton<std::map<std::string,
        //     float>&>("resourceLoader.resourcesProgress");

        ImGui::ProgressBar(totalProgress);
        ImGui::Text("Loading... %.2f%%", totalProgress * 100.0f);
        // ImGui::ProgressBar(currentProgress);
        // ImGui::Text("Current... %.2f%%", currentProgress * 100.0f);

        //         for (auto& [resource, progress] : resourcesProgress)
        //         {
        //             ImGui::ProgressBar(progress);
        //             ImGui::Text("%s... %.2f%%", resource.c_str(), progress * 100.0f);
        //         }
        // make the group fit the contents vertically

        ImGui::EndGroup();
        ImGui::End();
        if (totalProgress >= 1.0f)
        {
            getGame().getDispatcher().trigger<pg::game::events::SwitchSceneEvent>({"galaxy"});
        }
    }
};

} // namespace galaxy::gui