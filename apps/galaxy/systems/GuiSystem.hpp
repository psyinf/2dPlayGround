#pragma once
#include <pgGame/systems/SystemInterface.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class GuiSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup() override
    {
        gui = std::make_unique<pg::Gui>(game.getApp());
        game.getApp().getEventHandler().setCallback([&](auto e) {
            gui->processEvent(e);
            return false;
        });
    }

    void handle(const pg::game::FrameStamp& frameStamp) override
    {
        gui->render([]() { ImGui::ShowDemoWindow(nullptr); });
    }

    std::unique_ptr<pg::Gui> gui;
};
} // namespace galaxy