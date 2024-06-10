#pragma once
#include <gui/GameGuiWidget.hpp>
#include <components/StarSystem.hpp>

namespace galaxy::gui {

class StatsWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw(pg::Gui& gui) override
    {
        ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("System Info");
        ImGui::Text(pos_str.data());
        ImGui::Text(system_state_str.data());
        ImGui::Text(faction_str.data());

        ImGui::End();
    }
};

} // namespace galaxy::gui