#include <SDLApp.h>
#include <SDLErrorTrace.h>



void textdemo_main() {

    pg::SDLApp    app{pg::config::WindowConfig{.screen{}, .offset{200, 200}, .size{1024,1024}}};


    auto render = [&](auto& app) {
    };

    auto done = false;
    app.loop(done, render);
}

int main(int argc, char** argv) {
    textdemo_main();
    return 0;
}