#pragma once
#include <pgGame/core/Scene.hpp>
#include <memory>

#include <pgEngine/core/FrameStamp.hpp>

#include <pgGame/components/singletons/RegisteredPreloaders.hpp>
#include <pgGame/core/ResourcePreLoader.hpp>
#include <gui/LoadResourcesWidget.hpp>

namespace galaxy {
// TODO: pre loader per scene-transition
class LoadResourcesScene : public pg::game::Scene
{
public:
    LoadResourcesScene(pg::game::Game& game, pg::game::SceneConfig&& scene_cfg, std::string sceneIdToPrepareFor)
      : pg::game::Scene(game, std::move(scene_cfg))
      , _sceneIdToPrepareFor(sceneIdToPrepareFor)
    {
    }

    // using pg::game::Scene::Scene;

    virtual ~LoadResourcesScene()
    {
        // wait for threads to finish
    }

    void start() override
    {
        if (started()) { return; }
        // add percentage of resources loaded as singleton
        addSingleton_as<float&>("resourceLoader.totalProgress", _percentTotalResourcesLoaded);
        addSingleton_as<std::map<std::string, float>&>("resourceLoader.resourcesProgress", _percentResourcesLoaded);

        // get all registered loaders
        // TODO: this might be per scene
        auto& loaders = getGame().getSingleton<pg::singleton::RegisteredLoaders>(_sceneIdToPrepareFor + ".loaders");
        _resourcePreloader.initialize(
            loaders.loaders, _percentResourcesLoaded, [this](float p) { _percentTotalResourcesLoaded = p; });
        setupOverlay();
        // load resources for coming scene(s)
        Scene::start();
    };

    void postFrame(pg::FrameStamp& /*frameStamp*/) override
    {
        float totalProgress = getGame().getCurrentScene().getSingleton<float&>("resourceLoader.totalProgress");
        if (totalProgress >= 1.0f)
        {
            getGame().getGlobalDispatcher().trigger<pg::game::events::SwitchSceneEvent>({_sceneIdToPrepareFor});
        }
    }

    void setupOverlay()
    {
        // update events
        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiBegin>(), pg::game::DRAWABLE_FIRST});

        pg::game::makeEntity<pg::game::GuiDrawable>(getSceneRegistry(),
                                                    {std::make_unique<pg::game::GuiEnd>(), pg::game::DRAWABLE_LAST});
        pg::game::makeEntity<pg::game::GuiDrawable>(
            getSceneRegistry(),
            {std::make_unique<galaxy::gui::LoadResourcesWidget>(getGame()), pg::game::DRAWABLE_DOCKING_AREA});
    }

private:
    float _percentTotalResourcesLoaded{};

    PercentCompletedMap _percentResourcesLoaded{};
    ResourcePreloader   _resourcePreloader;
    std::string         _sceneIdToPrepareFor;
};
} // namespace galaxy