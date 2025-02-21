

#pragma once
#include "imgui.h"
#include <pgFoundation/ScopeBeginEndWrapper.hpp>

// Specialization for ImGui::Begin to always call ImGui::End
template <>
struct pgf::AlwaysEnd<ImGui::Begin>
{
    constexpr static bool value = true;
};

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
bool WrappedBeginChild(const char*      str_id,
                       const ImVec2&    size = {},
                       ImGuiChildFlags  childFlags = 0,
                       ImGuiWindowFlags flags = 0)
{
    return ImGui::BeginChild(str_id, size, childFlags, flags);
}

bool WrappedTreeNode(const char* label)
{
    return ImGui::TreeNode(label);
}

using Window =
    ScopedBeginEnd<ImGui::Begin, ImGui::End, ParameterInstance<bool*, nullptr>, ParameterInstance<ImGuiWindowFlags, 0>>;

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

using TreeNode = ScopedBeginEnd<[](const char* label) { return ImGui::TreeNode(label); }, ImGui::TreePop>;
using Tree = VoidScopedBeginEnd<[](const char* label) { ImGui::TreePush(label); }, ImGui::TreePop>;

using PushId = VoidScopedBeginEnd<[](int id) { ImGui::PushID(id); }, ImGui::PopID>;
using PopUp = ScopedBeginEnd<ImGui::BeginPopup, ImGui::EndPopup, ParameterInstance<ImGuiWindowFlags, 0>>;

using PopUpModal = ScopedBeginEnd<ImGui::BeginPopupModal,
                                  ImGui::EndPopup,
                                  /*ParameterInstance<bool*, nullptr>,*/
                                  ParameterInstance<ImGuiWindowFlags, 0>>;

using PushItemWidth = VoidScopedBeginEnd<ImGui::PushItemWidth, ImGui::PopItemWidth, ParameterInstance<float, 0.0f>>;
} // namespace pgf::gui
