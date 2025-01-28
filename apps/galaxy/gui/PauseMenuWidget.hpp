#pragma once
#include <gui/GameGuiWidget.hpp>
#include <gui/menus/OptionsMenu.hpp>
#include <pgEngine/gui/GuiElements.hpp>
#include <pgGame/events/SceneManagementEvents.hpp>
#include <pgGame/events/GameEvents.hpp>

#include <pgEngine/gui/ImGuiScopedWrappers.hpp>
#include <events/UIEvents.hpp>
#include <imgui.h>

namespace galaxy::gui {

class PauseMenuWidget : public galaxy::gui::GameGuiWidget
{
public:
    PauseMenuWidget(pg::game::Game& game)
      : galaxy::gui::GameGuiWidget(game)
    {
    }

    std::string current_menu{};

    void onButtonPressed(const std::string& name, bool switchScene = true)
    {
        getGame().getDispatcher().trigger<galaxy::events::MenuButtonPressed>(
            {.menuName = "mainScreen", .buttonName = name});
        if (switchScene)
        {
            if (name == current_menu) { current_menu = {}; }
            else { current_menu = name; }
        }
    }

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        auto window = pgf::gui::Window("Pause Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        // Options
        if (ImGui::Button("Options")) { onButtonPressed("options"); }
        if (ImGui::Button("Close")) { onButtonPressed("close"); }

        if (current_menu == "options") { inGameOptions(getGame()); }
        if (current_menu == "close")
        {
            onButtonPressed("close", true);
            getGame().getCurrentScene().callSetter<bool>("galaxy.pause_menu_open", false);
        }
    }
};

} // namespace galaxy::gui