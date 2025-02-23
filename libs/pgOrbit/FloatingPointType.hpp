#pragma once
template <typename T>
concept FloatingPoint = std::is_same_v<T, float> || std::is_same_v<T, double>;
