#pragma once
#include <gui/GameGuiWidget.hpp>
#include <components/StarSystem.hpp>

namespace galaxy::gui {

class SystemInfoWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw(pg::Gui& gui) override
    {
        auto selected_entity = getGame().getSingleton_or<PickedEntity>("picked.entity", PickedEntity{}).entity;
        if (selected_entity == entt::null) { return; }
        auto&& [system, transform, faction] =
            getGame().getRegistry().get<galaxy::StarSystemState, pg::Transform2D, galaxy::Faction>(selected_entity);
        auto pos_str = fmt::format("Pos: {},{}", transform.pos[0], transform.pos[1]);
        auto system_state_str = fmt::format("State: {}", magic_enum::enum_name(system.colonizationStatus));
        auto faction_str = fmt::format("Faction: {}", faction.name);
        ImGui::Begin("System Info");
        ImGui::Text("System Info");
        ImGui::Text(pos_str.data());
        ImGui::Text(system_state_str.data());
        ImGui::Text(faction_str.data());

        ImGui::End();
    }
};

} // namespace galaxy::gui