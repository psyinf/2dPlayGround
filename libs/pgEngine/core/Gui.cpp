#include "Gui.hpp"
#include "App.hpp"

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
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    bindEventProcessing();

    // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    // dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;

    // ImGui::DockSpace(dockspace_id, ImVec2(10.0f, app.getWindowConfig().size[1]), dockspace_flags);
    // bool t = true;
    // ImGui::ShowDemoWindow(&t);
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

void pg::Gui::bindEventProcessing()
{
    app.getEventHandler().setCallback([&](auto e) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplSDL2_ProcessEvent(&e);
        // TODO: differentiate if event was mouse or keyboard
        if (io.WantCaptureMouse || io.WantCaptureKeyboard) { return true; }
        return false;
    });
}
