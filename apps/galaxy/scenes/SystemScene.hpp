#pragma once
#include <pgGame/core/Scene.hpp>
#include <pgGame/systems/SystemInterface.hpp>
#include <memory>

#include <pgEngine/core/LoadSave.hpp>
#include <pgEngine/resources/SpriteResource.hpp>

#include <gui/StatsWidget.hpp>
#include <gui/SystemInfo.hpp>
#include <gui/DashBoardWidget.hpp>

#include <gui/StarSystemWidget.hpp>
#include <gui/LoadResourcesWidget.hpp>

#include <serializer/ConfigSerializer.hpp>
#include <systems/GuiRenderSystem.hpp>
#include <renderables/Orbit.hpp>
#include <entt/core/hashed_string.hpp>
#include <components/Tags.hpp>

namespace galaxy {

template <uint32_t Name>
struct SystemIdTag
{
};

class SystemScene : public pg::game::Scene
{
public:
    SystemScene(pg::game::Game& game, pg::game::SceneConfig&& cfg)
      : pg::game::Scene(game, std::move(cfg))
    {
    }

    virtual ~SystemScene() = default;

    void start() override
    {
        if (!started())
        {
            setupOverlay();
            setupStarSystem();
            setupKeyHandler();
            Scene::start();
        }
        setupSystemsEntities();
    }

    void setupSystemsEntities()
    {
        auto selected_entity = getGame().getSingleton_or<PickedEntity>("picked.entity", PickedEntity{}).entity;
        if (selected_entity == entt::null) { return; }
        auto&& [system, transform, faction] =
            getGame().getGlobalRegistry().get<galaxy::StarSystemState, pg::Transform2D, galaxy::Faction>(
                selected_entity);
        // add tag based on system name, to find it later

        auto&& storage = getSceneRegistry().storage<void>(entt::hashed_string::value(system.name.c_str()));

        if (storage.size() > 0)
        {
            _selectedEntity = storage.at(0);
            getSceneRegistry().emplace<pg::tags::SelectedItemTag>(_selectedEntity);
            return;
        }
        else
        {
            auto entity = pg::game::makeEntity<pg::Transform2D,
                                               pg::game::Drawable,
                                               pg::game::RenderState,
                                               pg::tags::SystemRenderTag,
                                               pg::tags::SelectedItemTag>
                //
                (getSceneRegistry(),
                 {.pos{0, 0}, .scaleSpace{pg::TransformScaleSpace::World}},
                 pg::game::Drawable{std::make_unique<galaxy::OrbitRenderable>(
                     pg::randomBetween(100.0f, 200.0f), 1000, pg::Color{1, 1, 1, 1})},
                 {},
                 {},
                 {});
            storage.emplace(entity);
            _selectedEntity = entity;
        }
    }

    void stop() override
    {
        getSceneRegistry().remove<pg::tags::SelectedItemTag>(_selectedEntity);

        Scene::stop();
    }

private:
    void setupStarSystem()
    {
        auto selected_entity = getGame().getSingleton_or<PickedEntity>("picked.entity", PickedEntity{}).entity;
        if (selected_entity == entt::null) { return; }
        auto&& [system, transform, faction] =
            getGame().getGlobalRegistry().get<galaxy::StarSystemState, pg::Transform2D, galaxy::Faction>(
                selected_entity);
    }

    // TODO: default 3d key handler/camera
    void setupKeyHandler()
    {
        auto& game = getGame();
        addSingleton_as<pg::KeyStateMap&>("system.keyStateMap", getKeyStateMap());

        getKeyStateMap().registerMouseRelativeDraggedCallback([this](auto pos, auto state) {
            if (state & SDL_BUTTON_LMASK)
            {
                const auto absolute_drag_distance = pg::vec_cast<float>(pos);
                if (pg::lengthSquared(absolute_drag_distance) < 4.0f) { return; };
                getGlobalTransform().pos += absolute_drag_distance * (1.0f / getGlobalTransform().scale);
                isDragging = true;
            }
        });

        getKeyStateMap().registerMousePressedCallback([this](auto pos, auto button, bool pressed) {
            if (isDragging && !pressed)
            {
                isDragging = false;
                return;
            }
            if (button == SDL_BUTTON_LEFT && !pressed)
            {
                // fire a pick event. TODO: distinguish between drag and click
                auto world_pos = pg::vec_cast<float>(pos);
                auto windowRect = getSingleton<pg::game::WindowDetails>().windowRect;
                auto globalTransform = getGlobalTransform();
                world_pos -= pg::dimsFromRect<float>(windowRect) * 0.5f;
                world_pos = (world_pos) * (1.0f / globalTransform.scale);
                world_pos -= globalTransform.pos;
                auto event = galaxy::events::PickEvent{
                    .screen_position{pos}, .world_position{world_pos}, .scale{globalTransform.scale}};
                getGame().getDispatcher().trigger(event);
            };
        });

        getKeyStateMap().registerMouseWheelCallback([this](auto pos) {
            auto zoom = galaxyConfig.zoom;
            getGlobalTransform().scale *= static_cast<float>(std::pow(zoom.factor + 1.0f, pos[1]));
            getGlobalTransform().scale[0] = std::clamp(getGlobalTransform().scale[0], zoom.min, zoom.max);
            getGlobalTransform().scale[1] = std::clamp(getGlobalTransform().scale[1], zoom.min, zoom.max);
        });

        getKeyStateMap().registerKeyCallback(
            SDLK_ESCAPE,
            [this](auto) { getGame().getGlobalDispatcher().enqueue<pg::game::events::SwitchSceneEvent>({"galaxy"}); },
            true);

        // TODO: in Game class
        game.getApp().getEventHandler().windowEvent = [this](const SDL_WindowEvent e) {
            if (e.event == SDL_WINDOWEVENT_RESIZED)
            {
                auto& windowDetails = getSingleton<pg::game::WindowDetails>();
                windowDetails.windowRect.w = e.data1;
                windowDetails.windowRect.h = e.data2;
            }
        };

        // TODO: ESC/ESC
        //
    }

    void setupOverlay()
    {
        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiBegin>(), pg::game::DRAWABLE_FIRST});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiEnd>(), pg::game::DRAWABLE_LAST});

        pg::game::makeEntity<pg::game::GuiDrawable>(
            getSceneRegistry(),
            {std::make_unique<galaxy::gui::StarSystemWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});
    }

private:
    std::unique_ptr<pg::Quadtree<entt::entity>> galaxyQuadtree;
    config::Galaxy                              galaxyConfig;
    bool                                        isDragging{};
    entt::entity                                _selectedEntity{};
};

} // namespace galaxy