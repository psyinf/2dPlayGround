#pragma once
#include <pgGame/systems/SystemInterface.hpp>

namespace galaxy {
using entt::literals::operator""_hs;

class Game;

class GuiSystem : public pg::game::SystemInterface
{
public:
    using SystemInterface::SystemInterface;

    void setup() override {}

    void handle(const pg::game::FrameStamp& frameStamp) override
    {
        // TODO: iterate entities
        // gui->render([]() { ImGui::ShowDemoWindow(nullptr); });
    }

    std::unique_ptr<pg::Gui> gui;
};
} // namespace galaxy