#pragma once
#include <gui/GameGuiWidget.hpp>
#include <pgGame/events/SceneManagementEvents.hpp>
#include <imgui.h>

namespace galaxy::gui {

class SplashScreenWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void menuButton(const ImVec2& anchor, const std::string& name, std::function<void()> func = {})
    {
        auto current = ImGui::GetCursorPos();
        // std::cout << ImGui::GetCursorPosX() << std::endl;
        auto off_size_x = 25;
        auto off_size_y = 30;

        auto off_anchor = ImVec2(anchor.x + off_size_x, anchor.y);
        ImGui::GetWindowDrawList()->AddLine(anchor, off_anchor, IM_COL32(255, 255, 255, 255), 2.0f);
        // TODO: offsets as percentage of window size
        auto res = ImGui::Button(name.c_str(), ImVec2(200, 50));
        std::cout << ImGui::GetCursorPosX() << std::endl;
        // restore cursor position
        ImGui::SetCursorPos(ImVec2(current.x, ImGui::GetCursorPosY()));
        ImGui::GetWindowDrawList()->AddLine(
            off_anchor,
            ImVec2(ImGui::GetCursorPosX() - off_size_x, ImGui::GetCursorPosY() - off_size_y),
            IM_COL32(255, 255, 255, 255),
            2.0f);

        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(ImGui::GetCursorPosX() - off_size_x, ImGui::GetCursorPosY() - off_size_y),
            ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - off_size_y),
            IM_COL32(255, 255, 255, 255),
            2.0f);

        std::cout << ImGui::GetCursorPosX() << std::endl;
        if (res && func) { func(); }
    }

    void fitToScreen(int& size_x, int& size_y)
    {
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
    }

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        auto dot_texture =
            getGame().getCurrentScene().getTypedResourceCache<sdl::Texture>().load("../data/background/splash1.png");
        auto button_texture =
            getGame().getCurrentScene().getTypedResourceCache<sdl::Texture>().load("../data/UI/frame.png");

        int size_x, size_y;
        dot_texture->query(nullptr, nullptr, &size_x, &size_y);
        // stretch or squeeze the image to fit the screen
        fitToScreen(size_x, size_y);

        bool open = false;
        ImGui::Begin(
            "Welcome", &open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos(ImVec2(0, 0));

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
        // draw a line to the left of the buttons from 0,middle of the screen
        auto anchor = ImVec2(0, ImGui::GetWindowSize().y / 2);
        menuButton(anchor, "Start", [this]() {
            getGame().getDispatcher().enqueue<pg::game::events::SwitchSceneEvent>("galaxy");
        });
        auto current = ImGui::GetCursorPos();
        // add size of button
        current.x += 200;
        current.y += 25;
        menuButton(anchor, "Options", [this]() { options = true; });
        menuButton(anchor, "Help");
        ImGui::Dummy(ImVec2(0.0f, 100));

        menuButton(anchor, "About");
        ImGui::EndGroup();
        if (options)
        {
            ImGui::SetCursorPos(ImVec2(400, 50));
            ;
            menuButton(current, "knallo", [this]() { options = false; });
        }

        ImGui::End();

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(5);
    }

    bool options{};
};

} // namespace galaxy::gui