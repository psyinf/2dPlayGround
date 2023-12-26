#include <Factories.hpp>
#include <SDLApp.h>
#include <SDLErrorTrace.h>
#include <SDLPrimitives.h>
#include <States.hpp>
#include <FPSCounter.hpp>
#include <random>
//TODO: make Rect class represting pos, dimension? 

//TODO: move this to lib 
pg::fVec2 makeRandom(const pg::fVec2& minMaxX, const pg::fVec2& minMaxY)
{
    // use random distribution to generate random numbers between min and max
    static std::random_device                    rd;
    static std::mt19937                          gen(rd());
    static std::uniform_real_distribution<float> disX(minMaxX[0], minMaxX[1]);
    static std::uniform_real_distribution<float> disY(minMaxY[0], minMaxY[1]);

    return {disX(gen), disY(gen)};
}

void createStars(std::vector<pg::fVec2>& positions, size_t count, const pg::fVec2& pos, const pg::fVec2& width)
{
    positions.clear();
    positions.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        positions.emplace_back(makeRandom({pos[0], pos[0] + width[0]}, {pos[1], pos[1] + width[1]}));
    }
}

void textdemo_main()
{
    pg::SDLApp app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{1024, 1024}}};

    auto font = pg::SDLFont("../data/fonts/Roboto-Regular.ttf", 24);
    // make a text sprite
    auto dot = pg::SpriteFactory::makeSprite(app.getRenderer(), "../data/circle_05.png");
    auto line = pg::Line{{0, 0}, {100, 100}};

    auto rendererStates = pg::States{};
    rendererStates.push(pg::TextureColorState{pg::Color{255, 255, 0, 255}});
    rendererStates.push(pg::TextureBlendModeState{SDL_BLENDMODE_ADD});
    //
    std::vector<pg::fVec2> positions;
    auto                   bounds = app.getDisplayBounds(0);
    createStars(positions,
                1000,
                vec_cast<float>(pg::iVec2{bounds.x, bounds.y}),
                vec_cast<float>(pg::iVec2{bounds.w, bounds.h}));
    pg::FPSCounter fpsCounter; //move to app?
    auto render = [&](auto& app) {
       
        for (const auto& pos : positions)
        {
            dot.draw(app.getRenderer(), {.pos{pos}, .scale{0.05, 0.05}}, rendererStates);
        }
        fpsCounter.frame();
        if (fpsCounter.getCurrentFrameCount() % 100 == 0)
        {
            std::cout << fpsCounter.getAverageFPSAndReset() << "\n";
        }
    };

    auto done = false;
    app.loop(done, render);
}

int main(int argc, char** argv)
{
    textdemo_main();
    return 0;
}