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
        ImGui::Text("Time Passed: %f", timeStamp.getRealTimePassed_sec());
        ImGui::Text("Galaxy Time: %s", timeStamp.formatTime().c_str());

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
        // time acceleration
        if (ImGui::Button("1x"))
        {
            auto event = pg::game::events::TimeScaleEvent{.time_scale = 1.0f};
            getGame().getGlobalDispatcher().enqueue<pg::game::events::TimeScaleEvent>(event);
        }
        if (ImGui::Button("10x"))
        {
            auto event = pg::game::events::TimeScaleEvent{.time_scale = 10.0f};
            getGame().getGlobalDispatcher().enqueue<pg::game::events::TimeScaleEvent>(event);
        }
        if (ImGui::Button("100x"))
        {
            auto event = pg::game::events::TimeScaleEvent{.time_scale = 100.0f};
            getGame().getGlobalDispatcher().enqueue<pg::game::events::TimeScaleEvent>(event);
        }
        if (ImGui::Button("1000x"))
        {
            auto event = pg::game::events::TimeScaleEvent{.time_scale = 1000.0f};
            getGame().getGlobalDispatcher().enqueue<pg::game::events::TimeScaleEvent>(event);
        }

        ImGui::End();
    }
};

} // namespace galaxy::gui