#pragma once
#include <imgui.h>
#include <pgEngine/core/Gui.hpp>
#include <pgEngine/gui/GuiElements.hpp>
#include <pgEngine/gui/ImGuiScopedWrappers.hpp>

#include <string>
#include <functional>
#include <array>

// guiElements.hpp

namespace galaxy::gui {

static constexpr auto small_galaxy_stars = 1000;
static constexpr auto medium_galaxy_stars = 5000;
static constexpr auto large_galaxy_stars = 15000;
static constexpr auto huge_galaxy_stars = 25000;
static constexpr auto massive_galaxy_stars = 50000;
// array of names
static constexpr auto galaxy_star_sizes = std::to_array({"Small", "Medium", "Large", "Huge", "Massive"});
static constexpr auto galaxy_star_sizes_counts = std::to_array({1000u, 5000u, 10000u, 20000u, 50000u});

static auto getIndexFromStarCount(unsigned int count) -> int
{
    // use lower_bound to find the first element that is not less than count
    auto it = std::lower_bound(galaxy_star_sizes_counts.begin(), galaxy_star_sizes_counts.end(), count);
    return static_cast<int>(std::distance(galaxy_star_sizes_counts.begin(), it));
}

static std::string_view getNullSeparatedNames()
{
    static std::string result;
    if (result.empty())
    {
        for (auto& name : galaxy_star_sizes)
        {
            result += name;
            result += '\0';
        }
    }
    return result;
}

static void inGameOptions(pg::game::Game& game)
{
    auto opacity = game.getCurrentScene().callGetter<float>("galaxy.background.opacity");
    if (ImGui::SliderFloat("Opacity", &opacity, 0.f, 1.f))
    {
        game.getCurrentScene().callSetter<float>("galaxy.background.opacity", opacity);
    }

    if (game.getCurrentScene().hasAccessor<pg::Color>("galaxy.grid.color"))
    {
        pg::Color       gridColor = game.getCurrentScene().callGetter<pg::Color>("galaxy.grid.color");
        pg::Vec4<float> gridColorFloat = pg::vec_cast<float>(gridColor);
        gridColorFloat /= 255.f;

        if (ImGui::ColorEdit3("Grid Visibility", gridColorFloat.data()))
        {
            gridColor = pg::vec_cast<uint8_t>(gridColorFloat * 255.f);
            game.getCurrentScene().callSetter<pg::Color>("galaxy.grid.color", gridColor);
        }
    }

    auto volume = game.getCurrentScene().callGetter<float>("scene.sound.volume");
    if (ImGui::SliderFloat("Global Volume", &volume, 0.f, 1.f))
    {
        game.getCurrentScene().callSetter<float>("scene.sound.volume", volume);
    }
}

static bool optionsMenu(galaxy::config::Galaxy&                 galaxy_config,
                        bool&                                   closed,
                        std::function<void(const std::string&)> buttonPressed)
{
    // TODO: tabs

    static bool changes = false;
    bool        save = false;
    closed = false;
    using pg::gui::ButtonSize;
    ImGui::SetCursorPos(ImVec2(50, 50));
    {
        pgf::gui::Child  child("##Options" /*ImVec2(400, 400), true, ImGuiWindowFlags_NoScrollbar*/);
        pgf::gui::TabBar tab_bar("Options");
        if (auto item = pgf::gui::TabItem("Galaxy"); item.get())
        {
            ImGui::SeparatorText("View");
            // slider for opacity
            if (ImGui::SliderFloat("Background opacity", &galaxy_config.background.opacity, 0.0f, 1.0f))
            {
                changes = true;
            }
            // number of stars as small (1000), medium (5000), large (15000)
            auto current_item = getIndexFromStarCount(galaxy_config.creation.num_stars);
            // string from names
            // const char* data = galaxy_star_sizes.data();
            // horizontal line with name
            ImGui::SeparatorText("Galaxy");
            if (ImGui::Combo("Number of stars", &current_item, getNullSeparatedNames().data()))
            {
                changes = true;
                galaxy_config.creation.num_stars = galaxy_star_sizes_counts.at(current_item);
            }
            ImGui::SameLine();
            // star count as text
            ImGui::Text(": %d", galaxy_config.creation.num_stars);
        }
        if (auto item = pgf::gui::TabItem("Factions"); item.get())
        {
            // collapsible faction list
            ImGui::SeparatorText("Factions");
            // list of factions as tree
            // add button
            if (ImGui::Button("Activate Faction"))
            {
                // find first inactive faction
                auto it = std::find_if(galaxy_config.factions.begin(),
                                       galaxy_config.factions.end(),
                                       [](const auto& faction) { return !faction.active; });
                if (it != galaxy_config.factions.end())
                {
                    it->active = true;
                    changes = true;
                }
                else { ImGui::OpenPopup("No Inactive Factions"); }
                changes = true;
            }

            for (auto&& [id, faction] : std::views::enumerate(galaxy_config.factions))
            {
                // we need to copy the faction name to avoid treeNode close on edit
                if (!faction.active) { continue; }
                std::vector<char> name(256, '\0');
                std::copy_n(faction.name.begin(), faction.name.size(), name.begin());
                auto label_name = faction.name + "###" + std::to_string(id);
                // color to float
                auto color = ImVec4(faction.color[0] / 255.0f,
                                    faction.color[1] / 255.0f,
                                    faction.color[2] / 255.0f,
                                    faction.color[3] / 255.0f);

                if (auto node = pgf::gui::TreeNode(label_name.c_str()); node.get())
                {
                    // active
                    // button to deactivate faction
                    if (ImGui::Button("Deactivate Faction"))
                    {
                        faction.active = false;
                        changes = true;
                    }
                    // faction name
                    if (ImGui::InputText("Faction Name", name.data(), name.size()))
                    {
                        changes = true;
                        faction.name = name.data();
                    }
                    // faction color
                    if (ImGui::ColorEdit4("Color", &color.x))
                    {
                        changes = true;
                        faction.color = {static_cast<uint8_t>(color.x * 255),
                                         static_cast<uint8_t>(color.y * 255),
                                         static_cast<uint8_t>(color.z * 255),
                                         static_cast<uint8_t>(color.w * 255)};
                    }
                    // faction starting parameters
                    ImGui::SeparatorText("Starting Parameters");
                    // TODO: should support minutes/hours/ ...
                    if (ImGui::InputScalar(
                            "Starting delay (sec)", ImGuiDataType_U64, &faction.startParams.start_offset_seconds))
                    {
                        changes = true;
                    }
                    // number of drones
                    if (ImGui::InputScalar(
                            "Number of Starting Drones", ImGuiDataType_U8, &faction.startParams.num_start_drones))
                    {
                        changes = true;
                    }

                    // faction drone parameters
                    ImGui::SeparatorText("Drone Parameters");
                    // max acceleration
                    if (ImGui::SliderFloat("Max Acceleration (g)", &faction.droneParams.max_acceleration, 0.f, 1000.f))
                    {
                        changes = true;
                    }
                    // max speed
                    if (ImGui::SliderFloat("Max Speed (c)", &faction.droneParams.max_speed, 0.0f, 1.0f))
                    {
                        changes = true;
                    }
                    // max range
                    if (ImGui::SliderFloat("Max Range (ly)", &faction.droneParams.max_range, 0.0f, 100.0f))
                    {
                        changes = true;
                    }
                    // expected lifetime
                    if (ImGui::InputScalar(
                            "Expected Lifetime (sec)", ImGuiDataType_U32, &faction.droneParams.expected_lifetime))
                    {
                        changes = true;
                    }
                    // reliability factor
                    if (ImGui::SliderFloat("Reliability Factor", &faction.droneParams.reliability_factor, 0.0f, 1.0f))
                    {
                        changes = true;
                    }
                }
            }
        }

        // on save
        if (changes)
        {
            pg::gui::button<ButtonSize::Medium>("Save", [=](auto) {
                ImGui::OpenPopup("Save?");
                buttonPressed("Save");
            });
            ImGui::SameLine();
        }

        pg::gui::button<pg::gui::ButtonSize::Medium>("Close", [=, &closed](auto) {
            //
            if (changes) { ImGui::OpenPopup("Close?"); }
            else { closed = true; }
            buttonPressed("Close");
        });
        if (ImGui::BeginPopupModal("Save?"))
        {
            ImGui::Text("Save changes?");
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                buttonPressed("Saved");
                closed = true;
                save = true;
                changes = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                buttonPressed("Closed");
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal("Close?"))
        {
            ImGui::Text("Discard changes?");
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                closed = true;
                save = false;
                changes = false;
                ImGui::CloseCurrentPopup();
                buttonPressed("Closed");
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                buttonPressed("Closed");
            }
            ImGui::EndPopup();
        }
    }

    return save;
}
} // namespace galaxy::gui