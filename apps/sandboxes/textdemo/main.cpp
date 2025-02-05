#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/State.hpp>
#include <pgGame/core/Game.hpp>

#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/State.hpp>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

class RenderSystem : public pg::game::SystemInterface
{
    std::shared_ptr<pg::Renderable> _renderable;

    float _rot{};

public:
    using SystemInterface::SystemInterface;

    void setupImGui(sdl::Window& window, sdl::Renderer& renderer)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer bindings
        // window is the SDL_Window*
        // context is the SDL_GLContext
        ImGui_ImplSDL2_InitForSDLRenderer(window.get(), renderer.get());
        ImGui_ImplSDLRenderer2_Init(renderer.get());
    }

    void imGuiFrame([[maybe_unused]] sdl::Window& window, sdl::Renderer& renderer)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code
        // to learn more about Dear ImGui!).
        bool show_demo_window = true;

        ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Render();
        SDL_RenderSetScale(renderer.get(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        // SDL_SetRenderDrawColor(renderer.get(), (Uint8)(255), (Uint8)(255), (Uint8)(255), (Uint8)(255));
        // SDL_RenderClear(renderer.get());
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer.get());
    }

    void setup(std::string_view /*scene_id*/) override
    {
        auto font = pg::SDLFont("../data/fonts/Roboto-Regular.ttf", 24);
        _renderable = std::make_shared<pg::Sprite>(
            pg::SpriteFactory::makeTextSprite(_game.getApp().getRenderer(), font, "Hello, World!"));
    }

    void handle(const pg::FrameStamp& frameStamp) override
    {
        auto renderer = pg::Renderer{_game.getApp().getRenderer(), frameStamp};
        renderer.clear();

        auto rendererStates = pg::States{};
        auto rendererStates2 = pg::States{};
        auto rendererStates3 = pg::States{};

        rendererStates.push(pg::TextureColorState{pg::Color{255, 0, 0, 255}});
        rendererStates2.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
        rendererStates2.push(pg::TextureAlphaState{128});
        rendererStates3.push(pg::TextureColorState{pg::Color{255, 0, 255, 255}});
        pg::Renderer r{renderer.renderer, {}};
        auto&        app = _game.getApp();
        imGuiFrame(app.getWindow(), app.getRenderer());

        _renderable->draw(
            r,
            {.pos{100, 100}, .rotation_deg{_rot += 0.1f}, .scale{static_cast<float>(std::sin(_rot * 0.1f)), 0.5f}},
            rendererStates);

        _renderable->draw(r, {.pos{512, 512}}, rendererStates2);

        _renderable->draw(
            r,
            {.pos{200, 200}, .rotation_deg{_rot += 0.1f}, .scale{static_cast<float>(std::sin(_rot * 0.1f)), 0.5f}},
            rendererStates3);
        _renderable->draw(r, {.pos{512, 200}}, {});
        r.present();
    }
};

void textdemo_main()
{
    pg::game::Game game({});

    pg::game::SystemsFactory::registerSystem<RenderSystem>("renderSystem");

    game.createScene({.scene_id = "start", .systems = {"renderSystem"}});
    game.switchScene("start");

    game.loop();
}

/*
void textdemo_main()
{
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{1024, 1024}}};

    setupImGui(app.getWindow(), app.getRenderer());

    auto font = pg::SDLFont("../data/fonts/Roboto-Regular.ttf", 24);
    // make a text sprite
    auto text = pg::SpriteFactory::makeTextSprite(app.getRenderer(), font, "Hello World");
    auto text2 = pg::SpriteFactory::makeTextSprite(app.getRenderer(), font, "Hello World");
    // draw it
    float rot = 0.0;

    auto rendererStates = pg::States{};
    auto rendererStates2 = pg::States{};
    auto rendererStates3 = pg::States{};

    rendererStates.push(pg::TextureColorState{pg::Color{255, 0, 0, 255}});
    rendererStates2.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
    rendererStates2.push(pg::TextureAlphaState{128});
    rendererStates3.push(pg::TextureColorState{pg::Color{255, 0, 255, 255}});
    pg::Renderer r{app.getRenderer(), {}};
    auto         render = [&](auto& app) {
        imGuiFrame(app.getWindow(), app.getRenderer());

        text.draw(r,
                          {.pos{100, 100}, .rotation_deg{rot += 0.1f}, .scale{static_cast<float>(std::sin(rot * 0.1f)),
0.5f}}, rendererStates);

        text.draw(r, {.pos{512, 512}}, rendererStates2);

        text.draw(r,
                          {.pos{200, 200}, .rotation_deg{rot += 0.1f}, .scale{static_cast<float>(std::sin(rot * 0.1f)),
0.5f}}, rendererStates3); text.draw(r, {.pos{512, 200}}, {});
    };

    auto done = false;

    app.loop(done, render, [](const auto& evt) {
        ImGui_ImplSDL2_ProcessEvent(&evt);
        return false;
    });
}
*/
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    textdemo_main();
    return 0;
}