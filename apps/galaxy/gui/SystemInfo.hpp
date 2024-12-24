#pragma once
#include <gui/GameGuiWidget.hpp>
#include <components/StarSystem.hpp>
#include <components/PickedEntity.hpp>
#include <components/Faction.hpp>
#include <magic_enum.hpp>
#include <pgEngine/generators/MarkovNameGen.hpp>
#include <pgEngine/math/Trigonometric.hpp>

namespace galaxy::gui {

auto sectorNameFromAngle(float angle_deg) -> std::string
{
    // normalize angle to 0-360
    angle_deg = std::fmod(angle_deg + 360.0f, 360.0f);
    // 0° is up
    angle_deg += 90.0f;
    // every 30 degrees, starting at -15 degrees. First sector is alpha, last is omega
    static constexpr auto greek_letter_names =
        std::to_array({"alpha", "beta",  "gamma",  "delta",   "epsilon", "zeta", "eta",     "theta",
                       "iota",  "kappa", "lambda", "mu",      "nu",      "xi",   "omicron", "pi",
                       "rho",   "sigma", "tau",    "upsilon", "phi",     "chi",  "psi",     "omega"});
    auto sector = static_cast<int>((angle_deg - 15.0f) / 30.0f);

    return greek_letter_names.at(sector);
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

        // set up system name if not yet created
        if (system.name.empty())
        {
            pg::SeedGenerator seed_gen(entt::to_integral(selected_entity));
            const auto&       mfm = getGame().getSingleton<pg::generators::MarkovFrequencyMap<4>>("markovFrequencyMap");
            system.name = pg::generators::markov::generate<4>(3, 8, mfm, seed_gen);
        }
        auto polar_pos = pg::math::toPolar(transform.pos);
        // auto pos_str = fmt::format("Pos: {},{}", polar_pos[0], polarAngleToDeg(polar_pos[1]));
        //  name is polar lenght, sector name.remainder angle
        auto sector_name = sectorNameFromAngle(pg::math::toDegrees(std::get<1>(polar_pos)));
        auto sector_angle = inSectorAngle(std::get<1>(polar_pos));

        auto pos_str = fmt::format("{}:{}.{}", std::get<0>(polar_pos), sector_name, sector_angle);

        // auto pos_str = fmt::format("Pos: {},{}", transform.pos[0], transform.pos[1]);
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
        ImGui::Text(pos_str.data());
        ImGui::Text(system_state_str.data());
        ImGui::Text(faction_str.data());

        ImGui::Text(spectral_type.data());

        ImGui::End();
    }
};

} // namespace galaxy::gui