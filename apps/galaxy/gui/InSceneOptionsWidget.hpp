#pragma once
#include <gui/GameGuiWidget.hpp>
#include <pgGame/events/GameEvents.hpp>

namespace galaxy::gui {

class InSceneOptionsWidget : public galaxy::gui::GameGuiWidget
{
public:
    using galaxy::gui::GameGuiWidget::GameGuiWidget;

    void draw([[maybe_unused]] pg::Gui& gui) override
    {
        ImGui::Begin("In Scene Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        // HACK:
        if (getGame().getCurrentSceneId() == "galaxy")
        {
            // background opacity
            auto opacity = getGame().getCurrentScene().callGetter<float>("galaxy.background.opacity");
            if (ImGui::SliderFloat("Opacity", &opacity, 0.f, 1.f))
            {
                getGame().getCurrentScene().callSetter<float>("galaxy.background.opacity", opacity);
            }
            // grid visible
            if (getGame().getCurrentScene().hasAccessor<float>("galaxy.grid.opacity"))
            {
                float gridVisible = getGame().getCurrentScene().callGetter<float>("galaxy.grid.opacity");
                if (ImGui::SliderFloat("Grid Visibility", &gridVisible, 0.f, 1.f))
                {
                    getGame().getCurrentScene().callSetter<float>("galaxy.grid.opacity", gridVisible);
                }
            }
        }

        auto volume = getGame().getCurrentScene().callGetter<float>("scene.sound.volume");
        if (ImGui::SliderFloat("Global Volume", &volume, 0.f, 1.f))
        {
            getGame().getCurrentScene().callSetter<float>("scene.sound.volume", volume);
        }

        ImGui::End();
    }
};

} // namespace galaxy::gui