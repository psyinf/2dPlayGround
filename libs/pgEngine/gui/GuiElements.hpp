#pragma once
#include "imgui.h"

namespace pg::gui {
/**
 * A stack to restore previous GUI position cursor
 */
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

enum class ButtonSize
{
    Small,
    Medium,
    Large
};
// for now predefined
static const inline std::unordered_map<ButtonSize, ImVec2> buttonSizes = {
    {ButtonSize::Small, ImVec2(50, 50)},
    {ButtonSize::Medium, ImVec2(100, 0)},
    {ButtonSize::Large, ImVec2(200, 50)},
};

template <ButtonSize size = ButtonSize::Large>
static void button(const std::string& name, std::function<void(const std::string& name)> func = {})
{
    auto imSize = buttonSizes.at(size);
    auto current = ImGui::GetCursorPos();
    auto res = ImGui::Button(name.c_str(), imSize);
    if (res)
    {
        if (func) { func(name); }
    }
}

static void menuButton(const ImVec2&      anchor,
                       const std::string& name,

                       std::function<void(const std::string&)> func = {})
{
    static auto lineColor = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
    // ImU32{IM_COL32(255, 0.7 * 255, 0 * 255, 128)};

    pg::gui::GuiPosStack stack;
    auto                 current = ImGui::GetCursorPos();
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
        off_anchor, ImVec2(ImGui::GetCursorPosX() - off_size_x, ImGui::GetCursorPosY() - off_size_y), lineColor, 2.0f);

    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(ImGui::GetCursorPosX() - off_size_x, ImGui::GetCursorPosY() - off_size_y),
        ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - off_size_y),
        lineColor,
        2.0f);

    if (res)
    {
        if (func) { func(name); }
    }
}

static void fitToScreen(int& size_x, int& size_y)
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
static ImVec2 enforceMinimumSize(int& size_x, int& size_y, int min_x, int min_y)
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
} // namespace pg::gui
