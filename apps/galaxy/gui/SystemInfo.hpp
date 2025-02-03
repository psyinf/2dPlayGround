#pragma once
#include <gui/GameGuiWidget.hpp>
#include <components/StarSystem.hpp>
#include <components/PickedEntity.hpp>
#include <components/Faction.hpp>
#include <magic_enum.hpp>

namespace galaxy::gui {

class SystemInfoWidget : public galaxy::gui::GameGuiWidget
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

        auto name = buildSystemPositionStr(transform.pos);

        auto system_state_str = std::format("State: {}", magic_enum::enum_name(system.colonizationStatus));
        auto faction_str = std::format("Faction: {}", faction.name);
        auto system_name = std::format("Name: {}", system.name);
        auto spectral_type = std::format("Class: {}", magic_enum::enum_name(system.spectralType));
        ImGui::BeginMainMenuBar();
        // Add menu bar items here
        ImGui::EndMainMenuBar();

        ImGui::Begin("System Info", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("System Info");
        ImGui::Text(system_name.data());
        ImGui::Text(name.data());
        ImGui::Text(system_state_str.data());
        ImGui::Text(faction_str.data());

        ImGui::Text(spectral_type.data());

        ImGui::End();
    }
};

} // namespace galaxy::gui