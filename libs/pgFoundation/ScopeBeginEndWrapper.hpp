#pragma once

namespace pgf {
template <typename T, T value>
struct ParameterInstance
{
    static T get() { return T(value); }
};

template <auto begin, auto end, typename... BeginArgs>
class ScopedBeginEnd
{
public:
    template <typename... Args>
    ScopedBeginEnd(Args&&... args)
    {
        begin(std::forward<Args>(args)..., get_value<BeginArgs>()...);
    }

    ~ScopedBeginEnd() { end(); }

private:
    template <typename T>
    static auto get_value()
    {
        return T::get();
    }
};

/* Example usage :
* imagine a ImGui::BeginTabItem and ImGui::EndTabItem functions the need to be called in pairs
* where BeginTabBar's signature as follows:
* BeginTabItem(char* name, bool* p_open = 0, ImGuiTabItemFlags = 0)
* by using
* using TabItem = ScopedBeginEnd<ImGui::BeginTabItem,
                               ImGui::EndTabItem,
                               ParameterInstance<bool*, 0>,
                               ParameterInstance<ImGuiTabItemFlags, 0>>;
* we effectively can do
* {
*    TabItem tab("Tab1");
*  ...
* }
* and the destructor will call EndTabItem
*/

} // namespace pgf