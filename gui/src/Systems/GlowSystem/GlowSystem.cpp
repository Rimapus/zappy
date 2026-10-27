/*
** EPITECH PROJECT, 2026
** gui/src/Systems/GlowSystem/GlowSystem
** File description:
** GlowSystem source file
*/

#include "GlowSystem.hpp"
#include "../../Components/Glow.hpp"
#include "../../Components/Sprite.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include <cmath>

/**
 * @brief Linearly interpolate between two colors channel by channel.
 *
 * @param a      The color returned when t = 0
 * @param b      The color returned when t = 1
 * @param t      The blend factor in [0, 1]
 *
 * @return sf::Color The interpolated color
 */
static sf::Color lerpColor(const sf::Color &a, const sf::Color &b, float t)
{
    return sf::Color(
        static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
        static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
        static_cast<std::uint8_t>(a.b + (b.b - a.b) * t),
        static_cast<std::uint8_t>(a.a + (b.a - a.a) * t));
}

/**
 * @brief Advance every glow by dt and write the pulsing tint into each
 *        entity's Sprite before it is rendered.
 *
 * @param coordinator      The coordinator to access components
 * @param dt               Elapsed time since the last frame (seconds)
 */
void GlowSystem::update(Coordinator &coordinator, float dt)
{
    for (Entity const &entity : _entities) {
        Glow &glow = coordinator.getComponent<Glow>(entity);
        Sprite &sprite = coordinator.getComponent<Sprite>(entity);

        glow._elapsed += dt;
        float t = (std::sin(glow._elapsed * glow._speed) + 1.0f) * 0.5f;
        if (sprite._sprite.has_value() == true) {
            sprite._sprite->setColor(lerpColor(glow._base, glow._peak, t));
        }
    }
}
