#pragma once

#include "IconsFontAwesome6.h"
#include <gui/GameGuiWidget.hpp>
#include <imgui.h>
#include <pgGame/events/GameEvents.hpp>

namespace galaxy::gui {

class MainFrameWidget : public galaxy::gui::GameGuiWidget
{
private:
    void modifiyTimeScaleAndSend(float current_time_scale, bool positive)
    {
        // we want factors 0.1, 0.5, 1, 2, 10, 100, 1000, 10000
        static constexpr auto factors = {0.1f, 0.5f, 1.0f, 2.0f, 10.0f, 100.0f, 1000.0f, 10000.0f};
        // find the current factor by using lower_bound
        auto it = std::lower_bound(factors.begin(), factors.end(), current_time_scale);
        if (it == factors.end())
        {
            // if we are at the end, we are at the highest factor
            it = factors.end() - 1;
        }
        // if we want to increase the time scale, we want to go to the next factor
        if (positive)
        {
            it++;
            if (it == factors.end())
            {
                // if we are at the end, we are at the highest factor
                it = factors.end() - 1;
            }
        }
        else
        {
            if (it == factors.begin()) { return; }
            it--;
        }

        current_time_scale = *it;
        current_time_scale = std::clamp(current_time_scale, 0.1f, 10000.0f);

        auto event = pg::game::events::TimeScaleEvent{.time_scale = current_time_scale};
        getGame().getGlobalDispatcher().enqueue<pg::game::events::TimeScaleEvent>(event);
    };

    struct StarSystemDetails
    {
        StarSystemState state;
        pg::Transform2D trans;
        galaxy::Faction faction;
    };

    auto getCurrentSystem() -> StarSystemDetails
    {
        auto selected_entity = getGame().getSingleton_or<PickedEntity>("picked.entity", PickedEntity{}).entity;
        if (selected_entity == entt::null) { return StarSystemDetails{}; }
        auto&& [state, transform, faction] =
            getGame().getGlobalRegistry().get<galaxy::StarSystemState, pg::Transform2D, galaxy::Faction>(
                selected_entity);

        return StarSystemDetails{.state = state, .trans = transform, .faction = faction};
    }

public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw(pg::Gui& /*gui*/)
    {
        auto current_time_scale = getGame().getCurrentGameState().timeScale;
        auto timeStamp = getGame().getCurrentTimeStamp();
        ImGui::Begin("##",
                     nullptr,
                     ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::SetWindowPos(ImVec2(0, ImGui::GetCursorPosY() + 5));
        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 50));

        // disable button frame
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.1f);

        {
            ImGui::Columns(3, "", true);
            /// @separator

            /// @begin Text
            ImGui::Text(ICON_FA_SUN " %s", getCurrentSystem().state.name.data());
            auto details_text = fmt::format("Class:{}, Pos:{}",
                                            magic_enum::enum_name(getCurrentSystem().state.spectralType),
                                            galaxy::buildSystemPositionStr(getCurrentSystem().trans.pos));
            ImGui::Text(ICON_FA_MAGNIFYING_GLASS " %s", details_text.data());
            /// @end Text

            /// @begin Button
            ImGui::NextColumn();
            if (ImGui::Button(ICON_FA_BACKWARD)) { modifiyTimeScaleAndSend(current_time_scale, false); }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_PAUSE))
            {
                auto event = pg::game::events::PlayPauseEvent{.state = pg::game::events::PlayPauseEvent::State::Pause};
                getGame().getGlobalDispatcher().enqueue<pg::game::events::PlayPauseEvent>(event);
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_PLAY))
            {
                auto scale_event = pg::game::events::TimeScaleEvent{.time_scale = 1.0};
                getGame().getGlobalDispatcher().enqueue<pg::game::events::TimeScaleEvent>(scale_event);
                auto event = pg::game::events::PlayPauseEvent{.state = pg::game::events::PlayPauseEvent::State::Play};
                getGame().getGlobalDispatcher().enqueue<pg::game::events::PlayPauseEvent>(event);
                // speed to 1
            }
            ImGui::SameLine();
            if (ImGui::Button(ICON_FA_FORWARD)) { modifiyTimeScaleAndSend(current_time_scale, true); }
            /// @end Button

            /// @begin Text
            ImGui::NextColumn();
            ImGui::Text(ICON_FA_CLOCK " %s", timeStamp.formatTime().c_str());
            /// @end Text

            /// @begin Text
            ImGui::Text(ICON_FA_GAUGE ": %.2f", getGame().getCurrentGameState().timeScale);
            /// @end Text

            /// @separator
        }
        ImGui::PopStyleVar(1);
        ImGui::End();

        /// @end Child
    }
};

} // namespace galaxy::gui