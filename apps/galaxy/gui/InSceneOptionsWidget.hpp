#pragma once
#include <gui/GameGuiWidget.hpp>
#include <pgGame/events/GameEvents.hpp>
#include <pgEngine/gui/ImGuiScopedWrappers.hpp>
#include "menus/OptionsMenu.hpp"

namespace galaxy::gui {

class InSceneOptionsWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        if (getGame().getCurrentSceneId() != "galaxy") { return; }
        auto& menu_active = getGame().getCurrentScene().getSingleton<bool&>("galaxy.options_menu.active");
        if (!menu_active) { return; }
        {
            auto window = pgf::gui::Window("Options", &menu_active);
            inGameOptions(getGame());
        }
    }
};

} // namespace galaxy::gui