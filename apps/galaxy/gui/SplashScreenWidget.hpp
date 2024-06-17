#pragma once
#include <gui/GameGuiWidget.hpp>
#include <pgGame/events/SceneManagementEvents.hpp>
#include <imgui.h>

namespace galaxy::gui {

class SplashScreenWidget : public galaxy::gui::GameGuiWidget
{
    bool ImageButtonWithText(ImTextureID texture, const std::string& text, ImVec2 pos, ImVec2 dims)
    {
        ImGui::SetCursorPos(pos);
        bool clicked = ImGui::ImageButton(
            ("Button_" + text).c_str(), texture, dims, {0, 0}, {1, 1}, ImVec4{0, 0, 0, 1.0}, ImVec4{1, 1, 1, 1});
        ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
        ImGui::SetCursorPos({pos.x + (dims.x - text_size.x) * 0.5f, pos.y + (dims.y - text_size.y) * 0.5f});

        // ImGui::TextColored(ImVec4{0, 0, 0, 1}, text.c_str());
        return clicked;
    }

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
        ImGui::Begin("Welcome", &open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowPos(ImVec2(0, 0));
        // limit window size according to aspect ratio

        if (window_size.x / window_size.y > aspect_ratio)
        {
            ImGui::SetWindowSize(ImVec2(window_size.y * aspect_ratio, window_size.y));
        }
        else { ImGui::SetWindowSize(ImVec2(window_size.x, window_size.x / aspect_ratio)); }
        // background sprite

        ImGui::Image((void*)dot_texture.get()->get(), ImVec2(static_cast<float>(size_x), static_cast<float>(size_y)));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9f);
        ImGui::PushStyleColor(ImGuiCol_Button, {1, 0, 0, 1});

        // centered frame
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 100, ImGui::GetWindowSize().y / 2 - 50));
        ImGui::BeginGroup();

        // buttons

        if (ImGui::Button("Start", ImVec2(200, 50)))
        {
            getGame().getDispatcher().enqueue<pg::game::events::SwitchSceneEvent>("galaxy");
        }
        ImGui::Button("Options", ImVec2(200, 50));

        ImGui::End();
        ImGui::EndGroup();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
    }
};

} // namespace galaxy::gui