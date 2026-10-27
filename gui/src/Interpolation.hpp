/*
 * EPITECH PROJECT, 2026
 * Interpolation.hpp
 * File description:
 * Pure movement interpolation helpers (no SFML / no ECS dependency)
 */

#pragma once

#include "Components/Movement.hpp"
#include "Components/Transform.hpp"
#include <cmath>

/**
 * @brief Smoothstep easing (ease-in-out): maps t in [0,1] to a smoothed [0,1].
 *
 * @param t  Normalized progress, clamped to [0, 1].
 * @return   Eased progress in [0, 1].
 */
inline float easeInOut(float t)
{
    if (t <= 0.0f) {
        return 0.0f;
    }
    if (t >= 1.0f) {
        return 1.0f;
    }
    return t * t * (3.0f - 2.0f * t);
}

/**
 * @brief Arm a move toward a target, starting from the entity's current pixel.
 *
 * @param transform  Current position (read for the start point).
 * @param movement   Movement state to arm.
 * @param targetX    Destination X in pixels.
 * @param targetY    Destination Y in pixels.
 * @param duration   Time in seconds to reach the target (must be > 0).
 */
inline void startMove(const Transform &transform, Movement &movement, std::int32_t targetX, std::int32_t targetY,
                      float duration)
{
    movement._startX = transform._x;
    movement._startY = transform._y;
    movement._targetX = targetX;
    movement._targetY = targetY;
    movement._duration = duration > 0.0f ? duration : 1.0f;
    movement._progress = 0.0f;
}

/**
 * @brief Advance one in-flight move by dt and write the eased position into the Transform.
 *
 * @param transform  Transform to write the interpolated position into.
 * @param movement   Movement state to step.
 * @param dt         Elapsed time since last frame (seconds).
 */
inline void stepMove(Transform &transform, Movement &movement, float dt)
{
    if (movement._progress >= 1.0f) {
        return;
    }
    movement._progress += dt / movement._duration;
    if (movement._progress >= 1.0f) {
        movement._progress = 1.0f;
        transform._x = movement._targetX;
        transform._y = movement._targetY;
        return;
    }
    float e = easeInOut(movement._progress);
    transform._x = movement._startX +
                   static_cast<std::int32_t>(std::lround((movement._targetX - movement._startX) * e));
    transform._y = movement._startY +
                   static_cast<std::int32_t>(std::lround((movement._targetY - movement._startY) * e));
}
