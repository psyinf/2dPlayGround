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

            if (getGame().getCurrentScene().hasAccessor<pg::Color>("galaxy.grid.color"))
            {
                pg::Color       gridColor = getGame().getCurrentScene().callGetter<pg::Color>("galaxy.grid.color");
                pg::Vec4<float> gridColorFloat = pg::vec_cast<float>(gridColor);
                gridColorFloat /= 255.f;

                if (ImGui::ColorEdit3("Grid Visibility", gridColorFloat.data()))
                {
                    gridColor = pg::vec_cast<uint8_t>(gridColorFloat * 255.f);
                    getGame().getCurrentScene().callSetter<pg::Color>("galaxy.grid.color", gridColor);
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