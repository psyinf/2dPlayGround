#include <SDLErrorTrace.h>
#include <fmt/format.h>
#include <SDLBackgoundSprite.h>

int main(int argc, char** argv)
try
{
    //LOAD a image file
    auto sprite = pg::SpriteFactory::makeSprite("../data/meteorBrown_big1.png");
    sdl::Texture.query(sdl::Texture::F)
    //find contours based on alpha/key color
    //create a convex hull from the contours
    return 0;
}
catch (std::exception& e)
{
    fmt::print("Unhandled exception: {}\n", e.what());
    errorTrace::printErrorTrace();
    return -1;
}
catch (...)
{
    fmt::print("Unhandled exception");
    errorTrace::printErrorTrace();
    return -1;
}