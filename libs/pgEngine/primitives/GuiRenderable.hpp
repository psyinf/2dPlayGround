#pragma once

#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/Gui.hpp>

namespace pg::game {

class GuiValueAccessor
{
public:
    template <typename T>
    void add(const std::string& name, const T& value)
    {
        values[name] = value;
        getter[name] = [this, name]() -> std::any { return values.at(name); };
        setter[name] = [this, name](const std::any& value) { values[name] = value; };
    }

    template <typename T>
    void addGetter(const std::string& name, const std::function<T()>& getter)
    {
        this->getter[name] = [getter]() -> std::any { return getter(); };
    }

    template <typename T>
    T get(const std::string& name)
    {
        return std::any_cast<T>(getter[name]());
    }

    template <typename T>
    void set(const std::string& name, const T& value)
    {
        setter[name](value);
    }

private:
    std::unordered_map<std::string, std::function<std::any()>>            getter;
    std::unordered_map<std::string, std::function<void(const std::any&)>> setter;
    std::unordered_map<std::string, std::any>                             values;
};

class GuiRenderable
{
public:
    GuiRenderable() = default;

    GuiRenderable(GuiValueAccessor&& gui_value_accessor)
      : gva_(std::move(gui_value_accessor))
    {
    }

    virtual void draw(pg::Gui& gui) = 0;

    GuiValueAccessor& gva() { return gva_; }

private:
    GuiValueAccessor gva_;
};

class GuiBegin : public GuiRenderable
{
public:
    virtual void draw(pg::Gui& gui) override { gui.begin(); }
};

class GuiEnd : public GuiRenderable
{
public:
    virtual void draw(pg::Gui& gui) override { gui.end(); }
};
} // namespace pg::game
