#pragma once
#include <gui/GameGuiWidget.hpp>

namespace galaxy::gui {

class MainBarWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        auto timeStamp = getGame().getCurrentTimeStamp();
        auto gameStamp = getGame().getCurrentGameState();
        ImGui::BeginMainMenuBar();
        // Add menu bar items here
        ImGui::EndMainMenuBar();

        ImGui::Begin("System Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Main Info");
        ImGui::Text("Frame: %d", timeStamp.frameNumber);
        ImGui::Text("Game Tick: %d", timeStamp.gameTick);
        ImGui::Text("Seconds: %d", timeStamp.time.seconds);
        //         ImGui::Text(system_name.data());
        //         ImGui::Text(pos_str.data());
        //         ImGui::Text(system_state_str.data());
        //         ImGui::Text(faction_str.data());
        //
        //         ImGui::Text(spectral_type.data());

        ImGui::End();
    }
};

} // namespace galaxy::gui