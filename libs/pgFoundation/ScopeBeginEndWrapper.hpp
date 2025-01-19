#pragma once

namespace {

// Helper to drop the first N elements from a tuple
template <std::size_t N, typename Tuple, std::size_t... Idx>
auto drop_first_N_impl(Tuple&& t, std::index_sequence<Idx...>)
{
    return std::make_tuple(std::get<Idx + N>(std::forward<Tuple>(t))...);
}

// Main function to drop first N elements
template <std::size_t N, typename... Args>
auto drop_first_N(const std::tuple<Args...>& t)
{
    constexpr std::size_t TupleSize = sizeof...(Args);
    if constexpr (N >= TupleSize)
    {
        return std::tuple<>(); // Return an empty tuple if N >= tuple size
    }
    else { return drop_first_N_impl<N>(t, std::make_index_sequence<TupleSize - N>{}); }
}
} // namespace

namespace pgf {
template <typename T, T value>
struct ParameterInstance
{
    static T get() { return T(value); }
};

template <auto T>
struct AlwaysEnd
{
    static constexpr bool value = false;
};

template <auto begin, auto end, typename... DefaultArgs>
class [[nodiscard]] ScopedBeginEnd
{
public:
    template <typename... Args>
    ScopedBeginEnd(Args&&... args)
    {
        constexpr std::size_t num_to_drop = sizeof...(Args) - 1;
        auto                  arguments = std::make_tuple(args...);
        auto                  default_arguments = std::make_tuple(get_arg_value<DefaultArgs>()...);
        auto                  default_arguments_adjusted = drop_first_N<num_to_drop>(default_arguments);
        auto                  all_args = std::tuple_cat(arguments, default_arguments_adjusted);
        res = std::apply(begin, all_args);
    }

    ~ScopedBeginEnd()
    {
        if (res || AlwaysEnd<begin>::value) { end(); }
    }

    bool get() const { return res; }

private:
    bool res{false};

    template <typename T>
    static auto get_arg_value() -> decltype(T::get())
    {
        return T::get();
    }
};

template <auto begin, auto end, typename... DefaultArgs>
class [[nodiscard]] VoidScopedBeginEnd
{
public:
    template <typename... Args>
    VoidScopedBeginEnd(Args&&... args)
    {
        constexpr std::size_t num_to_drop = sizeof...(Args) - 1;
        auto                  arguments = std::make_tuple(args...);
        auto                  default_arguments = std::make_tuple(get_arg_value<DefaultArgs>()...);
        auto                  default_arguments_adjusted = drop_first_N<num_to_drop>(default_arguments);
        auto                  all_args = std::tuple_cat(arguments, default_arguments_adjusted);
        std::apply(begin, all_args);
    }

    ~VoidScopedBeginEnd() { end(); }

private:
    template <typename T>
    static auto get_arg_value()
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
*    if (TabItem tab("Tab1").get())
*    {
*        // do stuff
*    }
*  ...
* }
* and the destructor will call EndTabItem when the if-block ends and TabItem returned true
* For void wrapped functions, use VoidScopedBeginEnd
* e.g. using  = VoidScopedBeginEnd<[](const char* label) { ImGui::Begin(label); }, ImGui::TreePop>;
* we can do
* using Child = ScopedBeginEnd<WrappedBeginChild,
                             ImGui::EndChild,
                             ParameterInstance<ImVec2, ImVec2{}>,
                             ParameterInstance<ImGuiChildFlags, 0>,
                             ParameterInstance<ImGuiWindowFlags, 0>>;
* When calling Child child("##Options" /*ImVec2(400, 400), true, ImGuiWindowFlags_NoScrollbar* /);
* the destructor will call ImGui::EndChild() when the block ends
*/

} // namespace pgf