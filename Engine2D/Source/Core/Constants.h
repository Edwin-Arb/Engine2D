#pragma once
#include <cstdint>
#include <numbers>

/** Window dimensions in pixels. */
constexpr int32_t SCREEN_WIDTH = 1000;
constexpr int32_t SCREEN_HEIGHT = 1000;

/** Math constants: tolerance for float comparisons and pi. */
constexpr float EPSILON = 0.001f;
constexpr float PI = std::numbers::pi_v<float>;

/** Movement speed and size of the main square actor. */
constexpr float SQUARE_SPEED = 450.0f;
constexpr float SQUARE_SIZE_X = 200.0f;
constexpr float SQUARE_SIZE_Y = 200.0f;

/** Radius of the target circle actor. */
constexpr float CIRCLE_RADIUS = 20.0f;

/** Distance from the target at which arrival behavior starts slowing down. */
constexpr float SLOW_RADIUS = 100.0f;