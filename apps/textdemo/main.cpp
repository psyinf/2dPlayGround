
#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/State.hpp>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

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

void imGuiFrame(sdl::Window& window, sdl::Renderer& renderer)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to
    // learn more about Dear ImGui!).
    bool show_demo_window = true;

    ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    SDL_RenderSetScale(renderer.get(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    // SDL_SetRenderDrawColor(renderer.get(), (Uint8)(255), (Uint8)(255), (Uint8)(255), (Uint8)(255));
    // SDL_RenderClear(renderer.get());
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer.get());
}

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

    auto render = [&](auto& app) {
        imGuiFrame(app.getWindow(), app.getRenderer());

        text.draw(app.getRenderer(),
                  {.pos{100, 100}, .rotation_deg{rot += 0.1}, .scale{static_cast<float>(std::sin(rot * 0.1f)), 0.5f}},
                  rendererStates);

        text.draw(app.getRenderer(), {.pos{512, 512}}, rendererStates2);

        text.draw(app.getRenderer(),
                  {.pos{200, 200}, .rotation_deg{rot += 0.1f}, .scale{static_cast<float>(std::sin(rot * 0.1f)), 0.5f}},
                  rendererStates3);
        text.draw(app.getRenderer(), {.pos{512, 200}}, {});
    };

    auto done = false;

    app.loop(done, render, [](const auto& evt) {
        ImGui_ImplSDL2_ProcessEvent(&evt);
        return false;
    });
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    textdemo_main();
    return 0;
}