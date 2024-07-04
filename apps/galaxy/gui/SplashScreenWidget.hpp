#pragma once
#include <gui/GameGuiWidget.hpp>
#include <pgGame/events/SceneManagementEvents.hpp>
#include <pgGame/events/GameEvents.hpp>
#include <imgui.h>

namespace galaxy::gui {
class GuiPosStack
{
public:
    GuiPosStack() = default;

    void push() { stack.push_back(ImGui::GetCursorPos()); }

    void pop()
    {
        ImGui::SetCursorPos(top());
        stack.pop_back();
    }

    ImVec2 top() const { return stack.back(); }

private:
    std::vector<ImVec2> stack;
};

class SplashScreenWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    static constexpr auto lineColor = ImU32{IM_COL32(255, 0.7 * 255, 0 * 255, 128)};

    void menuButton(const ImVec2& anchor, const std::string& name, std::function<void()> func = {})
    {
        GuiPosStack stack;
        auto        current = ImGui::GetCursorPos();
        // std::cout << ImGui::GetCursorPosX() << std::endl;
        auto off_size_x = 25;
        auto off_size_y = 30;

        auto off_anchor = ImVec2(anchor.x + off_size_x, anchor.y);
        ImGui::GetWindowDrawList()->AddLine(anchor, off_anchor, lineColor, 2.0f);
        // TODO: offsets as percentage of window size

        auto res = ImGui::Button(name.c_str(), ImVec2(200, 50));
        // restore cursor position
        ImGui::SetCursorPos(ImVec2(current.x, ImGui::GetCursorPosY()));
        ImGui::GetWindowDrawList()->AddLine(
            off_anchor,
            ImVec2(ImGui::GetCursorPosX() - off_size_x, ImGui::GetCursorPosY() - off_size_y),
            lineColor,
            2.0f);

        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(ImGui::GetCursorPosX() - off_size_x, ImGui::GetCursorPosY() - off_size_y),
            ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - off_size_y),
            lineColor,
            2.0f);

        if (res && func) { func(); }
    }

    void fitToScreen(int& size_x, int& size_y)
    {
        auto window_size = ImGui::GetIO().DisplaySize;

        float aspect_ratio = static_cast<float>(size_x) / static_cast<float>(size_y);
        if (window_size.x / window_size.y >= aspect_ratio)
        {
            size_x = static_cast<int>(window_size.y * aspect_ratio);
            size_y = static_cast<int>(window_size.y);
        }
        else
        {
            size_y = static_cast<int>(window_size.x / aspect_ratio);
            size_x = static_cast<int>(window_size.x);
        }
    }

    // enforce minimum size, honoring aspect ratio, cutting off the image if necessary
    // return a vector to be used by ImGui::SetCursorPos to center the image, cutting off left/right or top/bottom
    ImVec2 enforceMinimumSize(int& size_x, int& size_y, int min_x, int min_y)
    {
        float aspect_ratio = static_cast<float>(size_x) / static_cast<float>(size_y);
        auto  window_size = ImGui::GetIO().DisplaySize;
        if (size_x < min_x)
        {
            size_x = min_x;
            size_y = static_cast<int>(size_x / aspect_ratio);
        }
        if (size_y < min_y)
        {
            size_y = min_y;
            size_x = static_cast<int>(size_y * aspect_ratio);
        }

        return ImVec2((window_size.x - size_x) / 2, (window_size.y - size_y) / 2);
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
        auto centering = enforceMinimumSize(size_x, size_y, 0, 600);
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
        ImGui::Image((void*)dot_texture.get()->get(), ImVec2(static_cast<float>(size_x), static_cast<float>(size_y)));
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
        menuButton(anchor, "Start", [this]() {
            getGame().getDispatcher().trigger<pg::game::events::SwitchSceneEvent>({"loadGalaxy"});
        });
        auto options_anchor = ImVec2(ImGui::GetCursorPosX() + 200, ImGui::GetCursorPosY() + 50);
        // add size of button

        menuButton(anchor, "Options", [this]() { active_menu = "options"; });

        menuButton(anchor, "Help");

        ImGui::Dummy(ImVec2(0.0f, 100));

        menuButton(anchor, "About", [this]() { active_menu = "about"; });
        menuButton(anchor, "Quit", [this]() { getGame().getDispatcher().enqueue<pg::game::events::QuitEvent>(); });
        ImGui::Dummy(ImVec2(0.0f, 0));
        ImGui::EndGroup();
        if (active_menu == "options")
        {
            if (0)
            {
                ImGui::SetCursorPos(ImVec2(300, 50));
                ImGui::BeginChild("Options", ImVec2(350, 400), true);

                ImGui::EndChild();
            }
            ImGui::SetCursorPos(ImVec2(400, 50));

            menuButton(options_anchor, "Close ", [this]() { active_menu = {}; });
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

    std::string active_menu{};
};

} // namespace galaxy::gui