#include <pgEngine/core/FPSCounter.hpp>
#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/States.hpp>
#include <random>

#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/State.hpp>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/State.hpp>
#include <pgGame/core/Game.hpp>

struct Particle
{
    pg::fVec2 pos{};
    pg::fVec2 dir{};
};

// TODO: make Rect class representing pos, dimension?
pg::fVec2 makeRandom(const pg::fVec2& minMaxX, const pg::fVec2& minMaxY)
{
    // use random distribution to generate random numbers between min and max
    static std::random_device             rd;
    static std::mt19937                   gen(rd());
    std::uniform_real_distribution<float> disX(minMaxX[0], minMaxX[1]);
    std::uniform_real_distribution<float> disY(minMaxY[0], minMaxY[1]);

    return {disX(gen), disY(gen)};
}

class RenderSystem : public pg::game::SystemInterface
{
    std::vector<Particle>           particles;
    std::shared_ptr<pg::Renderable> dot;

public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view /*scene_id*/) override
    {
        dot = std::make_shared<pg::Sprite>(
            pg::SpriteFactory::makeSprite(_game.getApp().getRenderer(), "::resources/circle_05.png"));
        // auto       line = pg::Line{{0, 0}, {100, 100}};
        auto windowSize = _game.getApp().getWindowConfig().size;
        createStars(150, {0, 0}, pg::vec_cast<float>(windowSize));
    }

    void handle(const pg::FrameStamp& frameStamp) override
    {
        auto windowSize = _game.getApp().getWindowConfig().size;
        auto renderer = pg::Renderer{_game.getApp().getRenderer(), frameStamp};
        auto rendererStates = pg::States{};
        rendererStates.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
        rendererStates.push(pg::TextureBlendModeState{SDL_BLENDMODE_ADD});
        //

        pg::Renderer r{_game.getApp().getRenderer(), {}};

        renderer.clear();
        auto& fpsCounter = _game.getApp().getFPSCounter();
        for (auto& particle : particles)
        {
            auto pos = particle.pos;
            dot->draw(r, {.pos{pos}, .scale{0.05f, 0.05f}}, rendererStates);
            // update
        }
        renderer.present();
        updateStars(particles, 1.0f / fpsCounter.getLastFrameFPS(), {0, 0, windowSize[0], windowSize[1]});

        if (fpsCounter.getCurrentFrameCount() % 500 == 0) { std::cout << fpsCounter.getAverageFPSAndReset() << "\n"; }
    }

    void createStars(size_t count, const pg::fVec2& pos, const pg::fVec2& width)

    {
        particles.clear();
        particles.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            particles.emplace_back(Particle                                                               //
                                   {makeRandom({pos[0], pos[0] + width[0]}, {pos[1], pos[1] + width[1]}), //
                                    {makeRandom({-5, 5}, {-5, 5})}});
        }
    }

    void updateStars(std::vector<Particle>& particles, float frameTime, const SDL_Rect& bounds)
    {
        for (auto& particle : particles)
        {
            particle.pos += particle.dir * frameTime;
        }
        // reflect on bounds
        for (auto& particle : particles)
        {
            if (particle.pos[0] < bounds.x || particle.pos[0] > bounds.w) { particle.dir[0] *= -1; }
            if (particle.pos[1] < bounds.y || particle.pos[1] > bounds.h) { particle.dir[1] *= -1; }
        }
        // attract to attractor
        for (auto& particle : particles)
        {
            for (auto& particle2 : particles)
            {
                if (&particle != &particle2)
                {
                    auto dir = particle2.pos - particle.pos;
                    auto len = pg::lengthSquared(dir);
                    if (len > 1.0f) { particle.dir += pg::makeNormal(dir) * (0.5f / len); }
                }
            }
        }
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

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    textdemo_main();
    return 0;
}