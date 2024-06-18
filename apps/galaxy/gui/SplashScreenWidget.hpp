#pragma once
#include <gui/GameGuiWidget.hpp>
#include <pgGame/events/SceneManagementEvents.hpp>
#include <imgui.h>

namespace galaxy::gui {

class SplashScreenWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        auto dot_texture =
            getGame().getCurrentScene().getTypedResourceCache<sdl::Texture>().load("../data/background/splash1.png");
        auto button_texture =
            getGame().getCurrentScene().getTypedResourceCache<sdl::Texture>().load("../data/UI/frame.png");

        int size_x, size_y;
        dot_texture->query(nullptr, nullptr, &size_x, &size_y);

        // stretch or squeeze the image to fit the screen
        auto window_size = ImGui::GetIO().DisplaySize;

        float aspect_ratio = static_cast<float>(size_x) / static_cast<float>(size_y);
        if (window_size.x / window_size.y >= aspect_ratio)
        {
            size_x = window_size.y * aspect_ratio;
            size_y = window_size.y;
        }
        else
        {
            size_y = window_size.x / aspect_ratio;
            size_x = window_size.x;
        }

        bool open = false;
        ImGui::Begin(
            "Welcome", &open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos(ImVec2(0, 0));
        // limit window size according to aspect ratio
        if (window_size.x / window_size.y > aspect_ratio)
        {
            ImGui::SetWindowSize(ImVec2(window_size.y * aspect_ratio, window_size.y));
        }
        else { ImGui::SetWindowSize(ImVec2(window_size.x, 100 + window_size.x / aspect_ratio)); }

        // center vertically
        ImGui::SetCursorPos(ImVec2(0, 50));
        // background sprite
        ImGui::Image((void*)dot_texture.get()->get(), ImVec2(static_cast<float>(size_x), static_cast<float>(size_y)));
        // TODO: style from config

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9f);
        ImGui::PushStyleColor(ImGuiCol_Border, {0.8, 0.6, 0, 1});
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, {0.6, 0.4, 0, 1});

        ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.1, 0.2, 1});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2, 0.3, 1});
        // button text color orange
        ImGui::PushStyleColor(ImGuiCol_Text, {1.0, 0.7, 0.0, 1});
        // centered frame
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 20, ImGui::GetWindowSize().y / 6));
        ImGui::BeginGroup();

        // buttons

        if (ImGui::Button("Start", ImVec2(200, 50)))
        {
            getGame().getDispatcher().enqueue<pg::game::events::SwitchSceneEvent>("galaxy");
        }
        ImGui::Button("Options", ImVec2(200, 50));
        ImGui::Button("Help", ImVec2(200, 50));
        ImGui::Dummy(ImVec2(0.0f, 100));
        ImGui::Button("About", ImVec2(200, 50));

        ImGui::End();
        ImGui::EndGroup();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
    }
};

} // namespace galaxy::gui