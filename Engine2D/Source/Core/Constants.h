#pragma once
#include <cstdint>
#include <numbers>

/** Screen projection configuration bounds dimensions. */
constexpr int32_t SCREEN_WIDTH = 1000;
constexpr int32_t SCREEN_HEIGHT = 1000;

/** Numerical math precision thresholds constraints. */
constexpr float EPSILON = 0.001f;
constexpr float PI = std::numbers::pi_v<float>;

/** Main primary actor spatial speed configurations. */
constexpr float SQUARE_SPEED = 450.0f;
constexpr float SQUARE_SIZE_X = 200.0f;
constexpr float SQUARE_SIZE_Y = 200.0f;

/** Secondary destination target spatial dimensions. */
constexpr float CIRCLE_RADIUS = 20.0f;

/** Precision arrival behavior smoothing attenuation constraints limits. */
constexpr float SLOW_RADIUS = 100.0f;