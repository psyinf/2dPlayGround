

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

// we need to wrap BeginChild due to function overloading
void WrappedBeginChild(const char*      str_id,
                       const ImVec2&    size = {},
                       ImGuiChildFlags  childFlags = 0,
                       ImGuiWindowFlags flags = 0)
{
    ImGui::BeginChild(str_id, size, childFlags, flags);
}

using Child = ScopedBeginEnd<WrappedBeginChild,
                             ImGui::EndChild,
                             ParameterInstance<ImVec2, ImVec2{}>,
                             ParameterInstance<ImGuiChildFlags, 0>,
                             ParameterInstance<ImGuiWindowFlags, 0>>;
using TabItem = ScopedBeginEnd<ImGui::BeginTabItem,
                               ImGui::EndTabItem,
                               ParameterInstance<bool*, nullptr>,
                               ParameterInstance<ImGuiTabItemFlags, 0>>;
using TabBar = ScopedBeginEnd<ImGui::BeginTabBar, ImGui::EndTabBar, ParameterInstance<ImGuiTabBarFlags, 0>>;

} // namespace pgf::gui