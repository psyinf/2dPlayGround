#pragma once
#include <imgui.h>
#include <pgEngine/core/Gui.hpp>
#include <pgEngine/guiElements/guiElements.hpp>

// guiElements.hpp

namespace galaxy::gui {

static bool optionsMenu(galaxy::config::Galaxy&                 galaxy_config,
                        bool&                                   closed,
                        std::function<void(const std::string&)> buttonPressed)
{
    bool save = false;
    closed = false;
    using pg::gui::ButtonSize;
    ImGui::SetCursorPos(ImVec2(50, 50));
    // menuButton(options_anchor, "Close ", [this]() { active_menu = {}; });
    ImGui::BeginChild("Options");

    // slider for opacity
    ImGui::SliderFloat("Background opacity", &galaxy_config.background.opacity, 0.0f, 1.0f);
    // number of stars as small (1000), medium (5000), large (15000)
    auto current_item = galaxy_config.creation.num_stars <= 1000 ? 0 : galaxy_config.creation.num_stars <= 5000 ? 1 : 2;

    if (ImGui::Combo("Number of stars", &current_item, "Small\0Medium\0Large\0\0"))
    {
        switch (current_item)
        {
        case 0:
            galaxy_config.creation.num_stars = 1000;
            break;
        case 1:
            galaxy_config.creation.num_stars = 5000;
            break;
        case 2:
            galaxy_config.creation.num_stars = 15000;
            break;
        }
    }
    // star size

    // on save
    pg::gui::button<ButtonSize::Medium>("Save", [=](auto) {
        //
        ImGui::OpenPopup("Save?");

        buttonPressed("Save");
    });
    ImGui::SameLine();
    pg::gui::button<pg::gui::ButtonSize::Medium>("Close", [=](auto) {
        //
        ImGui::OpenPopup("Close?");
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

    ImGui::EndChild();

    return save;
}
} // namespace galaxy::gui