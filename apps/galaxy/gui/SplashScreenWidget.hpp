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

class SplashScreenWidget : public galaxy::gui::GameGuiWidget
{
public:
    SplashScreenWidget(pg::game::Game& game)
      : galaxy::gui::GameGuiWidget(game)
    {
        pg::load("../data/galaxy_config.json", galaxy_config);
    }

    void onButtonPressed(const std::string& name)
    {
        getGame().getDispatcher().trigger<galaxy::events::MenuButtonPressed>(
            {.menuName = "mainScreen", .buttonName = name});
    }

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        using pg::gui::ButtonSize;
        auto pressedFunc = [this](const std::string& name) { onButtonPressed(name); };
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
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
        // center vertically
        ImGui::SetCursorPos(ImVec2(centering.x, (ImGui::GetIO().DisplaySize.y * 0.5f) - (size_y * 0.5f)));
        // background sprite
        ImGui::Image((ImTextureID)dot_texture.get()->get(),
                     ImVec2(static_cast<float>(size_x), static_cast<float>(size_y)));
        // TODO: style from config

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9f);
        ImGui::PushStyleColor(ImGuiCol_Border, {0.8f, 0.6f, 0, 1});
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, {0.6f, 0.4f, 0, 1});

        ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.1f, 0.2f, 1});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2f, 0.3f, 1});
        // button text color orange
        ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.7f, 0.0f, 1});
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

        pg::gui::menuButton(anchor, "Options", [this](auto) {
            if (active_menu == "options")
            {
                active_menu = {};
                return;
            }
            active_menu = "options";
        });

        pg::gui::menuButton(anchor, "Help");

        ImGui::Dummy(ImVec2(0.0f, 100));

        pg::gui::menuButton(anchor, "About", pressedFunc);
        pg::gui::menuButton(
            anchor, "Quit", [this](auto name) { getGame().getDispatcher().enqueue<pg::game::events::QuitEvent>(); });
        ImGui::Dummy(ImVec2(0.0f, 0));
        ImGui::EndGroup();
        if (active_menu == "options")
        {
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
            });
            ImGui::SameLine();
            pg::gui::button<pg::gui::ButtonSize::Medium>("Close", [this](auto) {
                //
                ImGui::OpenPopup("Close?");
            });

            if (ImGui::BeginPopupModal("Save?"))
            {
                ImGui::Text("Save changes?");
                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    active_menu = {};
                    pg::save("../data/galaxy_config.json", galaxy_config);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
            if (ImGui::BeginPopupModal("Close?"))
            {
                ImGui::Text("Discard changes?");
                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    active_menu = {};
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            ImGui::EndChild();

            // frame
        }

        else if (active_menu == "about")
        {
            ImGui::SetCursorPos(ImVec2(300, 50));
            auto about_open = true;
            if (ImGui::Begin("About", &about_open))
            {
                ImGui::TextWrapped("Galaxy is a game about space exploration and colonization.");
                ImGui::End();
            }
            if (!about_open) { active_menu = {}; }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
        ImGui::End();
    }

    std::string            active_menu{};
    galaxy::config::Galaxy galaxy_config;
};

} // namespace galaxy::gui