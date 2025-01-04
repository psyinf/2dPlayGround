

#pragma once
#include "imgui.h"
#include <pgFoundation/ScopeBeginEndWrapper.hpp>

namespace pgf::gui {

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

using TabItem = ScopedBeginEnd<ImGui::BeginTabItem,
                               ImGui::EndTabItem,
                               ParameterInstance<bool*, 0>,
                               ParameterInstance<ImGuiTabItemFlags, 0>>;
using TabBar = ScopedBeginEnd<ImGui::BeginTabBar, ImGui::EndTabBar, ParameterInstance<ImGuiTabBarFlags, 0>>;

} // namespace pgf::gui