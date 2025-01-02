#pragma once
#include <gui/GameGuiWidget.hpp>
#include <pgGame/events/SceneManagementEvents.hpp>
#include <pgGame/events/GameEvents.hpp>
#include <events/UIEvents.hpp>
#include <imgui.h>
#include <sndX/BackgroundPlayer.hpp>

#include <unordered_map>
#include <pgEngine/guiElements/guiElements.hpp>

static soundEngineX::BackgroundPlayer _bgPlayer;

namespace galaxy::gui {

class StyleStack
{
public:
    ~StyleStack()
    {
        ImGui::PopStyleVar(static_cast<int>(_styles));
        ImGui::PopStyleColor(static_cast<int>(_colors));
    }

    void pushStyle(ImGuiStyleVar style, float value)
    {
        _styles++;
        ImGui::PushStyleVar(style, value);
    }

    void pushStyle(ImGuiStyleVar style, const ImVec2& value)
    {
        _styles++;
        ImGui::PushStyleVar(style, value);
    }

    void pushStyleColor(ImGuiCol style, const ImVec4& value)
    {
        _colors++;
        ImGui::PushStyleColor(style, value);
    }

private:
    size_t _styles{0};
    size_t _colors{0};
};

class MainMenuWidget : public galaxy::gui::GameGuiWidget
{
public:
    MainMenuWidget(pg::game::Game& game)
      : galaxy::gui::GameGuiWidget(game)
    {
        pg::load("../data/galaxy_config.json", galaxy_config);
    }

    void onButtonPressed(const std::string& name, bool switchScene = true)
    {
        getGame().getDispatcher().trigger<galaxy::events::MenuButtonPressed>(
            {.menuName = "mainScreen", .buttonName = name});
        if (switchScene)
        {
            if (name == active_menu) { active_menu = {}; }
            else { active_menu = name; }
        }
    }

    void handleOptionsMenu()
    {
        using pg::gui::ButtonSize;
        ImGui::SetCursorPos(ImVec2(50, 50));
        // menuButton(options_anchor, "Close ", [this]() { active_menu = {}; });
        ImGui::BeginChild("Options");

        // slider for opacity
        ImGui::SliderFloat("Background opacity", &galaxy_config.background.opacity, 0.0f, 1.0f);
        // number of stars as small (1000), medium (5000), large (15000)
        auto current_item = galaxy_config.creation.num_stars <= 1000   ? 0
                            : galaxy_config.creation.num_stars <= 5000 ? 1
                                                                       : 2;

        ;
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
        pg::gui::button<ButtonSize::Medium>("Save", [this](auto) {
            //
            ImGui::OpenPopup("Save?");
            onButtonPressed("Save");
        });
        ImGui::SameLine();
        pg::gui::button<pg::gui::ButtonSize::Medium>("Close", [this](auto) {
            //
            ImGui::OpenPopup("Close?");
            onButtonPressed("Close");
        });

        if (ImGui::BeginPopupModal("Save?"))
        {
            ImGui::Text("Save changes?");
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                onButtonPressed("Saved");
                active_menu = {};
                pg::save("../data/galaxy_config.json", galaxy_config);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                onButtonPressed("Closed");
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopupModal("Close?"))
        {
            ImGui::Text("Discard changes?");
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                active_menu = {};
                ImGui::CloseCurrentPopup();
                onButtonPressed("Closed");
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                onButtonPressed("Closed");
            }
            ImGui::EndPopup();
        }

        ImGui::EndChild();

        // frame
    }

    void handleHelpMenu()
    {
        ImGui::SetCursorPos(ImVec2(300, 50));
        auto is_open = true;
        // stretch the window

        if (ImGui::Begin("Help", &is_open))
        {
            ImGui::SetWindowSize(ImVec2(400, 0));
            ImGui::TextWrapped("No content yet");
        }
        ImGui::End();
        if (!is_open) { active_menu = {}; }
    }

    void handleAboutMenu()
    {
        ImGui::SetCursorPos(ImVec2(300, 50));
        auto is_open = true;
        // stretch the window

        if (ImGui::Begin("About", &is_open))
        {
            ImGui::SetWindowSize(ImVec2(400, 0));
            ImGui::TextWrapped("Galaxy is a game about space exploration and colonization.");
        }
        ImGui::End();
        if (!is_open) { active_menu = {}; }
    }

    void handleMenu(const std::string& name)
    {
        if (name == "options") { handleOptionsMenu(); }
        if (name == "help") { handleHelpMenu(); }
        if (name == "about") { handleAboutMenu(); }
    }

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        // TODO: save internally
        auto dot_texture = getGame().getResource<sdl::Texture>("../data/background/splash1.png");
        auto button_texture = getGame().getResource<sdl::Texture>("../data/UI/frame.png");

        int size_x, size_y;
        dot_texture->query(nullptr, nullptr, &size_x, &size_y);
        // stretch or squeeze the image to fit the screen
        pg::gui::fitToScreen(size_x, size_y);
        auto centering = pg::gui::enforceMinimumSize(size_x, size_y, 0, 600);
        bool open = false;
        ImGui::Begin("Welcome",
                     &open,
                     ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
            // center vertically
            ImGui::SetCursorPos(ImVec2(centering.x, (ImGui::GetIO().DisplaySize.y * 0.5f) - (size_y * 0.5f)));
            // background sprite
            ImGui::Image((ImTextureID)dot_texture.get()->get(),
                         ImVec2(static_cast<float>(size_x), static_cast<float>(size_y)));
            // TODO: style from config
            StyleStack stack;
            stack.pushStyle(ImGuiStyleVar_FrameRounding, 10.0f);
            stack.pushStyle(ImGuiStyleVar_FrameBorderSize, 2.0f);

            stack.pushStyle(ImGuiStyleVar_Alpha, 0.9f);
            stack.pushStyleColor(ImGuiCol_Border, {0.8f, 0.6f, 0, 1});
            stack.pushStyleColor(ImGuiCol_BorderShadow, {0.6f, 0.4f, 0, 1});

            stack.pushStyleColor(ImGuiCol_Button, {0, 0.1f, 0.2f, 1});
            stack.pushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2f, 0.3f, 1});
            // button text color orange
            stack.pushStyleColor(ImGuiCol_Text, {1.0f, 0.7f, 0.0f, 1});
            // centered frame
            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 10, ImGui::GetWindowSize().y / 6));
            ImGui::BeginGroup();

            // buttons
            // draw a line to the left of the buttons from 0,middle of the screen
            auto anchor = ImVec2(0, ImGui::GetWindowSize().y / 2);
            pg::gui::menuButton(anchor, "Start", [this](auto name) {
                getGame().getGlobalDispatcher().trigger<pg::game::events::SwitchSceneEvent>({"loadGalaxy"});
            });
            auto options_anchor = ImVec2(ImGui::GetCursorPosX() + 200, ImGui::GetCursorPosY() + 50);
            // add size of button

            pg::gui::menuButton(anchor, "Options", [this](auto) { onButtonPressed("options"); });

            pg::gui::menuButton(anchor, "Help", [this](auto) { onButtonPressed("help"); });

            ImGui::Dummy(ImVec2(0.0f, 100));

            pg::gui::menuButton(anchor, "About", [this](auto) { onButtonPressed("about", true); });
            pg::gui::menuButton(anchor, "Quit", [this](auto) {
                onButtonPressed("quit", false);
                getGame().getGlobalDispatcher().enqueue<pg::game::events::QuitEvent>();
            });
            ImGui::Dummy(ImVec2(0.0f, 0));
            ImGui::EndGroup();

            // draw the active menu
            if (!active_menu.empty()) { handleMenu(active_menu); }
        }
        ImGui::End();
    }

    std::string            active_menu{};
    galaxy::config::Galaxy galaxy_config;
};

} // namespace galaxy::gui