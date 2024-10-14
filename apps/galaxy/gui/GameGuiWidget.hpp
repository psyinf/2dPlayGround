#pragma once
#include <pgEngine/primitives/GuiRenderable.hpp>

namespace galaxy::gui {
class GameGuiWidget : public pg::game::GuiRenderable
{
public:
    GameGuiWidget(pg::game::Game& game_)
      : pg::game::GuiRenderable()
      , game(game_)
    {
    }

    pg::game::Game& getGame() { return game; }

private:
    pg::game::Game& game;
};

} // namespace galaxy::gui