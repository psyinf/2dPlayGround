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
        if (pgf::gui::TabItem("Galaxy").get())
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
        if (pgf::gui::TabItem("Factions").get())
        {
            // collapsible faction list
            ImGui::SeparatorText("Factions");
            // list of factions as tree

            for (auto&& [id, faction] : std::views::enumerate(galaxy_config.factions))
            {
                // we need to copy the faction name to avoid treeNode close on edit

                std::vector<char> name(256, '\0');
                std::copy_n(faction.name.begin(), faction.name.size(), name.begin());
                auto label_name = faction.name + "###" + std::to_string(id);
                // color to float
                auto color = ImVec4(faction.color[0] / 255.0f,
                                    faction.color[1] / 255.0f,
                                    faction.color[2] / 255.0f,
                                    faction.color[3] / 255.0f);

                if (pgf::gui::TreeNode(label_name.c_str()).get())
                {
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
                    // faction icon
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