#include "Gui.hpp"
#include "App.hpp"

bool pg::Gui::processEvent(const SDL_Event& event)
{
    return ImGui_ImplSDL2_ProcessEvent(&event);
}

pg::Gui::~Gui()
{
    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui::DestroyContext();
}

pg::Gui::Gui(SDLApp& app)
  : app(app)
{
    auto& window = app.getWindow();
    auto& renderer = app.getRenderer();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    // TODO:config
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // window is the SDL_Window*
    // context is the SDL_GLContext
    ImGui_ImplSDL2_InitForSDLRenderer(window.get(), renderer.get());
    ImGui_ImplSDLRenderer2_Init(renderer.get());
}

void pg::Gui::begin()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void pg::Gui::end()
{
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();

    SDL_RenderSetScale(app.getRenderer().get(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    // SDL_SetRenderDrawColor(renderer.get(), (Uint8)(255), (Uint8)(255), (Uint8)(255), (Uint8)(255));
    // SDL_RenderClear(renderer.get());
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), app.getRenderer().get());
}

void pg::Gui::render(std::function<void()> rendering)
{
    ImGuiIO& io = ImGui::GetIO();

    if (rendering) { rendering(); }
}
