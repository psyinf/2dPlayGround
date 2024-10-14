#pragma once
#include <gui/GameGuiWidget.hpp>
#include <imgui.h>
#include <components/Stats.hpp>
#include <ranges>

namespace galaxy::gui {

class StarSystemWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        // top bar with stats
        ImGui::Begin("Ressources", nullptr, ImGuiWindowFlags_MenuBar);
        ImGui::Text("res");

        // end

        ImGui::Text("Max FPS: %.2f", 8.0f);
        ImGui::End();
    }
};

} // namespace galaxy::gui