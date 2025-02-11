#pragma once
#include <gui/GameGuiWidget.hpp>
#include <imgui.h>

namespace galaxy::gui {

class StarSystemWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        auto selected_entity = getGame().getSingleton_or<PickedEntity>("picked.entity", PickedEntity{}).entity;
        if (selected_entity == entt::null) { return; }
        auto&& [system, transform, faction] =
            getGame().getGlobalRegistry().get<galaxy::StarSystemState, pg::Transform2D, galaxy::Faction>(
                selected_entity);

        // window fixed on the top line, not movable

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Star System",
                     nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoSavedSettings);

        std::vector<std::pair<std::string, float>> resources{{"iron", 10.f}, {"water", 29.25f}};
        ImGui::Text(system.name.data());
        ImGui::BeginTable("Resources", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg);
        for (auto&& [name, amount] : resources)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text(name.data());
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", amount);
        }
        ImGui::EndTable();
        ImGui::End();
    }
};

} // namespace galaxy::gui