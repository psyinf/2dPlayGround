#pragma once
#include <pgEngine/primitives/GuiRenderable.hpp>
#include <pgEngine/math/Transform.hpp>
#include <pgEngine/core/States.hpp>

namespace galaxy {

class DemoToolBox : public pg::game::GuiRenderable
{
public:
    using pg::game::GuiRenderable::GuiRenderable;

    DemoToolBox() {}

    virtual void draw(pg::Gui&) override
    {
        gva().add("my_tool_active", true);
        auto my_tool_active = gva().get<bool>("my_tool_active");
        auto entity = gva().get<entt::entity>("testEntity");
        ImGui::Begin("My First Tool", my_tool_active ? &my_tool_active : nullptr, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O"))
                { /* Do stuff */
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                { /* Do stuff */
                }
                if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Edit a color stored as 4 floats
        ImGui::ColorEdit4("Color", my_color);

        // Generate samples and plot them
        float samples[100];
        for (int n = 0; n < 100; n++)
            samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
        ImGui::PlotLines("Samples", samples, 100);

        // Display contents in a scrolling region
        // ImGui::TextColored(ImVec4(1, 1, 0, 1), fmt:);
        auto entity_int = (int)entt::to_integral(entity);
        ImGui::InputInt("entity", &entity_int);
        ImGui::BeginChild("Scrolling");
        for (int n = 0; n < 50; n++)
            ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();
        ImGui::End();
        if (!my_tool_active)
        { //
            gva().set("my_tool_active", my_tool_active);
        }
    }

    // bool  my_tool_active{true};
    float my_color[4]{0.0f, 0.0f, 0.0f, 1.0f};
};
} // namespace galaxy