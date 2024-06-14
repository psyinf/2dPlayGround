#pragma once
#include <gui/GameGuiWidget.hpp>
#include <imgui.h>
#include <components/Stats.hpp>
#include <ranges>

namespace galaxy::gui {

class StatsWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        auto& stats = getGame().getSingleton<const Stats&>("galaxy.stats");

        ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_MenuBar);
        ImGui::Text("Stats");
        for (const auto& faction : std::views::keys(stats.factionStats))
        {
            ImGui::Text("%s", faction.data());
            ImGui::Indent();
            for (const auto& [name, value] : stats.factionStats)
            {
                ImGui::Text("Drones created: %d", value.numDronesCreated);
                ImGui::Text("Drones failed: %d", value.numDronesFailed);
            }
            ImGui::Unindent();
            ImGui::Separator();
        }

        ImGui::SeparatorText("Galaxy");
        ImGui::Text("Systems: %d", stats.galaxyStats.numSystems);

        ImGui::End();
    }
};

} // namespace galaxy::gui