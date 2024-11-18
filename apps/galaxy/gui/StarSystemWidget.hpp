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
        // window fixed on the top line, not moveable

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Star System",
                     nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoSavedSettings);

        std::vector<std::pair<std::string, float>> resources{{"iron", 10.f}, {"water", 29.25f}};
        ImGui::Text("                ");
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