#include "RenderSystem.hpp"
#include <components/StarSystem.hpp>
#include <pgGame/components/Drawable.hpp>
#include <pgGame/core/Game.hpp>
#include <pgGame/components/WindowDetails.hpp>
#include <pgEngine/math/VecUtils.hpp>
#include <components/Tags.hpp>
#include <components/Drone.hpp>
#include <components/Faction.hpp>
#include <pgGame/components/RenderState.hpp>

void galaxy::RenderSystem::setup() {}

/// TODO: drawables should carry their respective states. This is a temporary solution which determines the state by the
/// type of entities

void galaxy::RenderSystem::handle(const pg::FrameStamp& fs)
{
    auto renderer = pg::Renderer{game.getApp().getRenderer(), fs};
    renderer.clear();
    auto rendererStates = pg::States{};
    // rendererStates.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
    rendererStates.push(pg::TextureBlendModeState{SDL_BLENDMODE_ADD});
    renderer.apply(rendererStates);
    auto windowRect = game.getCurrentScene().getSingleton<pg::game::WindowDetails>().windowRect;

    auto globalTransform = game.getCurrentScene().getSingleton<pg::Transform2D>(pg::game::Scene::GlobalTransformName);

    // drones
    // non-debug, generic items

    for (auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D, pg::game::RenderState>(
             entt::exclude<pg::tags::DebugRenderingItemTag>);
         auto& entity : view)
    {
        auto&& [drawable, transform, renderState] =
            view.get<pg::game::Drawable, pg::Transform2D, pg::game::RenderState>(entity);

        rendererStates.push_states(renderState.states);
        auto new_transform = transform;
        new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
        new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
        new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
        if (transform.scaleSpace == pg::TransformScaleSpace::World) { new_transform.scale *= globalTransform.scale; }
        drawable.prim->draw(renderer, new_transform, rendererStates);
        rendererStates.pop_states(renderState.states);
    }
    // TODO: global config for debug items

    // debug items, TODO: check why this not drawn reliably over other items
    //     if (game.getCurrentScene().getSingleton_<const
    //     galaxy::config::Galaxy&>("galaxy.config").debugging.draw_debug_items)
    //     {
    //         for (auto view = game.getRegistry().view<pg::game::Drawable, pg::Transform2D,
    //         pg::tags::DebugRenderingItemTag>(
    //                  entt::exclude<StarSystemState>);
    //              auto& entity : view)
    //         {
    //             auto&& [drawable, transform] = view.get<pg::game::Drawable, pg::Transform2D>(entity);
    //             auto new_transform = transform;
    //             new_transform.pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
    //             new_transform.pos = (globalTransform.pos + transform.pos) * globalTransform.scale;
    //             new_transform.pos += pg::dimsFromRect<float>(windowRect) * 0.5f;
    //             new_transform.scale *= globalTransform.scale;
    //             drawable.prim->draw(renderer, new_transform, rendererStates);
    //         }
    //     }
    renderer.restore(rendererStates);

    drawOverlays();

    renderer.present();
}

void galaxy::RenderSystem::drawOverlays()
{
    auto& gui = game.getGui();
    game.getRegistry().sort<pg::game::GuiDrawable>(
        [](const auto& lhs, const auto& rhs) { return lhs.order < rhs.order; }); // sort by Z-axis
    auto view = game.getRegistry().view<pg::game::GuiDrawable>();
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
        drawable.prim->draw(gui);
    }
    ImGui::PopStyleVar(7);
    ImGui::PopStyleColor(6);
}
