#pragma once
#include <gui/GameGuiWidget.hpp>
#include <pgGame/events/GameEvents.hpp>

namespace galaxy::gui {

class MainBarWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        // make
        auto timeStamp = getGame().getCurrentTimeStamp();
        auto gameStamp = getGame().getCurrentGameState();
        ImGui::BeginMainMenuBar();
        // Add menu bar items here
        ImGui::EndMainMenuBar();

        ImGui::Begin("System Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Main Info");
        ImGui::Text("Frame: %d", timeStamp.frameNumber);
        ImGui::Text("Game Tick: %d", timeStamp.gameTick);
        ImGui::Text("Seconds: %f", timeStamp.time.getFractionalSeconds());
        ImGui::Text("Time Passed: %f", timeStamp.getRealTimePassed());

        // add some play/pause button
        // play/pause event

        if (ImGui::Button("Pause"))
        {
            auto event = pg::game::events::PlayPauseEvent{.state = pg::game::events::PlayPauseEvent::State::Pause};
            getGame().getGlobalDispatcher().enqueue<pg::game::events::PlayPauseEvent>(event);
        }
        if (ImGui::Button("Play"))
        {
            auto event = pg::game::events::PlayPauseEvent{.state = pg::game::events::PlayPauseEvent::State::Play};
            getGame().getGlobalDispatcher().enqueue<pg::game::events::PlayPauseEvent>(event);
        }
        // display second based time as

        // TODO: structured time/GalaxyTime
        //         ImGui::Text(system_name.data());
        //         ImGui::Text(pos_str.data());
        //         ImGui::Text(system_state_str.data());
        //         ImGui::Text(faction_str.data());
        //
        //         ImGui::Text(spectral_type.data());

        ImGui::End();
    }
};

} // namespace galaxy::gui