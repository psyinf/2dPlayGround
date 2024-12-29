#pragma once
#include <gui/GameGuiWidget.hpp>
#include <components/StarSystem.hpp>
#include <components/PickedEntity.hpp>
#include <components/Faction.hpp>
#include <magic_enum.hpp>
#include <pgEngine/generators/MarkovNameGen.hpp>
#include <pgEngine/math/Trigonometric.hpp>
#include <pgEngine/core/Constants.hpp>

namespace galaxy::gui {

auto sectorNameFromAngle(float angle_deg) -> std::string
{ // 0° is up
    angle_deg += 90.0f;
    // adjust for sector 0 starting at -15 degrees
    angle_deg += 15.0f;

    // normalize angle to 0-360
    angle_deg = std::fmod(angle_deg + 360.0f, 360.0f);

    // every 30 degrees, starting at -15 degrees. First sector is alpha, last is omega

    // first sector is between -15 and +15
    auto sector = static_cast<int>(std::floor((angle_deg) / 30.0f));

    return pg::constants::greek_letter_names.at(sector);
}

// the remainder-angle inside a sector
auto inSectorAngle(float angle_deg) -> float
{
    return std::fmod(angle_deg + 15.0f, 30.0f);
}

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

        auto system_state_str = fmt::format("State: {}", magic_enum::enum_name(system.colonizationStatus));
        auto faction_str = fmt::format("Faction: {}", faction.name);
        auto system_name = fmt::format("Name: {}", system.name);
        auto spectral_type = fmt::format("Class: {}", magic_enum::enum_name(system.spectralType));
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

    auto buildSystemPositionStr(pg::fVec2 pos) -> std::string
    {
        auto polar_pos = pg::math::toPolar(pos);

        auto sector_name = sectorNameFromAngle(pg::math::toDegrees(std::get<1>(polar_pos)));
        auto sector_angle = 10 * inSectorAngle(std::get<1>(polar_pos));
        // name is sector - angle_in_sector:distance
        return fmt::format("{}-{:.0f}:{:.0f}", sector_name, sector_angle, 10 * std::get<0>(polar_pos));
    }
};

} // namespace galaxy::gui