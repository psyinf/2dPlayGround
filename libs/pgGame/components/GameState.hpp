#pragma once

namespace pg::game {

enum class PauseState
{
    Running,
    Paused,
};

struct GameState
{
    PauseState pauseState{PauseState::Paused};
    float      timeScale{1.0f};
};
} // namespace pg::game
