#include <pgEngine/core/FPSCounter.hpp>
#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/core/ErrorTrace.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/States.hpp>
#include <random>

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

struct Particle
{
    pg::fVec2 pos{};
    pg::fVec2 dir{};
};

void createStars(std::vector<Particle>& particles, size_t count, const pg::fVec2& pos, const pg::fVec2& width)
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

void updateStars(std::vector<Particle>& particles, float frameTime, const SDL_Rect& bounds, const pg::fVec2& attractor)
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

void textdemo_main()
{
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{10, 10}, .size{1024, 512}}};

    auto font = pg::SDLFont("../data/fonts/Roboto-Regular.ttf", 24);
    // make a text sprite
    auto dot = pg::SpriteFactory::makeSprite(app.getRenderer(), "../data/circle_05.png");
    auto line = pg::Line{{0, 0}, {100, 100}};

    auto rendererStates = pg::States{};
    rendererStates.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
    rendererStates.push(pg::TextureBlendModeState{SDL_BLENDMODE_ADD});
    //
    std::vector<Particle> particles;
    auto                  windowSize = app.getWindowConfig().size;
    createStars(particles, 150, {0, 0}, pg::vec_cast<float>(windowSize));
    // TODO: entt based rendering and update

    auto render = [&](auto& app) {
        auto& fpsCounter = app.getFPSCounter();
        for (auto& particle : particles)
        {
            auto pos = particle.pos;
            dot.draw(app.getRenderer(), {.pos{pos}, .scale{0.05, 0.05}}, rendererStates);
            // update
        }

        updateStars(
            particles, 1.0 / fpsCounter.getLastFrameFPS(), {0, 0, windowSize[0], windowSize[1]}, particles[0].pos);

        if (fpsCounter.getCurrentFrameCount() % 100 == 0) { std::cout << fpsCounter.getAverageFPSAndReset() << "\n"; }
    };

    auto done = false;
    app.loop(done, render);
}

int main(int argc, char** argv)
{
    textdemo_main();
    return 0;
}