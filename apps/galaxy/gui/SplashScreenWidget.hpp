#pragma once
#include <gui/GameGuiWidget.hpp>
#include <imgui.h>

namespace galaxy::gui {

class SplashScreenWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        bool open = false;
        ImGui::Begin("Welcome", &open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
        // background sprite

        auto dot_texture =
            getGame().getCurrentScene().getTypedResourceCache<sdl::Texture>().load("../data/background/splash1.png");

        int size_x, size_y;
        dot_texture->query(nullptr, nullptr, &size_x, &size_y);
        auto texture = dot_texture.get();

        //             if (my_image_texture == 0)
        //             {
        //                 my_image_texture = gui.loadTexture("assets/splash.png");
        //             }
        ImGui::Image((void*)dot_texture.get()->get(), ImVec2(size_x, size_y));
        // center text
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - ImGui::CalcTextSize("Welcome to Galaxy!").x / 2,
                                   ImGui::GetWindowSize().y / 2));
        ImGui::Text("Welcome to Galaxy!");
        ImGui::End();
    }
};

} // namespace galaxy::gui