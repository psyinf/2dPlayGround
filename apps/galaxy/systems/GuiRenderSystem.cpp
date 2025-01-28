#include "GuiRenderSystem.hpp"

#include <pgGame/core/Game.hpp>
#include <pgGame/components/Drawable.hpp>

void galaxy::GuiRenderSystem::setup(std::string_view /*scene_id*/) {}

void galaxy::GuiRenderSystem::handle(const pg::FrameStamp& /*frameStamp*/)
{
    auto& renderer = _game.getApp().getRenderer();
    if (getConfig().isSet("standalone")) { renderer.clear(); }
    auto& gui = _game.getGui();

    _game.getCurrentSceneRegistry().sort<pg::game::GuiDrawable>(
        [](const auto& lhs, const auto& rhs) { return lhs.order < rhs.order; }); // sort by Z-axis
    auto view = _game.getCurrentSceneRegistry().view<pg::game::GuiDrawable>();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    // all roundings
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9f);
    ImGui::PushStyleColor(ImGuiCol_Border, {0, 0.1f, 0.2f, 0.4f});
    ImGui::PushStyleColor(ImGuiCol_BorderShadow, {0.6f, 0.4f, 0, 1});

    ImGui::PushStyleColor(ImGuiCol_Button, {0, 0.1f, 0.2f, 1});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0, 0.2f, 0.3f, 1});
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {0, 0.1f, 0.2f, 0.4f});

    // button text color orange
    ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.7f, 0.0f, 1});
    for (const auto& entity : view)
    {
        auto& drawable = view.get<pg::game::GuiDrawable>(entity);
        if (!drawable.active) { continue; }
        drawable.prim->draw(gui);
    }
    ImGui::PopStyleVar(7);
    ImGui::PopStyleColor(6);

    renderer.present();
}
