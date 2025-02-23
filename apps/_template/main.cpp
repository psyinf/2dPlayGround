
#include <pgEngine/factories/Factories.hpp>
#include <pgEngine/core/App.hpp>
#include <pgEngine/primitives/Renderable.hpp>
#include <pgEngine/core/State.hpp>
#include <pgGame/core/Game.hpp>

class RenderSystem : public pg::game::SystemInterface
{
    struct PositionedRenderable
    {
        pg::Transform2D                 trans;
        std::shared_ptr<pg::Renderable> renderable;
    };

    using Renderables = std::vector<PositionedRenderable>;
    Renderables _renderables;

public:
    using SystemInterface::SystemInterface;

    void setup(std::string_view /*scene_id*/) override
    {
        auto font = pg::SDLFont("../data/fonts/Roboto-Regular.ttf", 24);
        _renderables.push_back({{.pos{100, 100}},
                                std::make_shared<pg::Sprite>(pg::SpriteFactory::makeTextSprite(
                                    _game.getApp().getRenderer(), font, "Hello, World!"))});
        _renderables.push_back({{.pos{100, 400}}, std::make_shared<pg::Line>(pg::fVec2{0, 0}, pg::fVec2{100, 100})});
    }

    void handle(const pg::FrameStamp& frameStamp) override
    {
        auto renderer = pg::Renderer{_game.getApp().getRenderer(), frameStamp};
        renderer.clear();
        // color state
        pg::ColorState colorState({0xff, 0x00, 0xff, 0xff});
        colorState.apply(renderer.renderer);

        for (auto& renderable : _renderables)
        {
            renderable.renderable->draw(renderer, renderable.trans, {});
        }
        colorState.restore(renderer.renderer);
        renderer.present();
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
