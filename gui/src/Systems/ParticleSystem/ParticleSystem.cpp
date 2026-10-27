/*
** EPITECH PROJECT, 2026
** gui/src/Systems/ParticleSystem/ParticleSystem
** File description:
** ParticleSystem source file
*/

#include "ParticleSystem.hpp"
#include "../../Components/Lifetime.hpp"
#include "../../Components/Particle.hpp"
#include "../../Components/Transform.hpp"
#include "../../Components/Velocity.hpp"
#include "../../Constants.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include <numbers>
#include <random>

/**
 * @brief Integrate all particle velocities, draw them.
 *        Destruction is delegated to LifetimeSystem.
 *
 * @param coordinator The coordinator to access components and resources
 * @param dt Elapsed time since the last frame (seconds)
 */
void ParticleSystem::update(Coordinator &coordinator, float dt)
{
    sf::RenderWindow &window = coordinator.getResource<sf::RenderWindow>();

    for (Entity const &entity : _entities) {
        Particle &p = coordinator.getComponent<Particle>(entity);
        Velocity &v = coordinator.getComponent<Velocity>(entity);
        Transform &t = coordinator.getComponent<Transform>(entity);
        Lifetime &lt = coordinator.getComponent<Lifetime>(entity);

        t._x += static_cast<std::int32_t>(v._vx * dt);
        t._y += static_cast<std::int32_t>(v._vy * dt);
        v._vx *= v._slow;
        v._vy *= v._slow;

        float life = lt._remaining / lt._total;
        float r = p._radius * life;

        sf::CircleShape circle(r);
        circle.setOrigin({r, r});
        circle.setPosition({static_cast<float>(t._x), static_cast<float>(t._y)});
        sf::Color c = p._color;
        c.a = static_cast<std::uint8_t>(255.f * life * life);
        circle.setFillColor(c);
        window.draw(circle);
    }
}

/**
 * @brief Spawn an expanding ring of particles around a player on expulsion.
 *
 * @param coordinator The coordinator to create entities into
 * @param cx World X center of the expelling player
 * @param cy World Y center of the expelling player
 * @param color Team color applied to the particles
 */
void ParticleSystem::spawnShockwave(Coordinator &coordinator, float cx, float cy, float dirAngle, sf::Color color)
{
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> jitter(0.85f, 1.15f);

    for (int i = 0; i < SHOCKWAVE_PARTICLE_COUNT; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(SHOCKWAVE_PARTICLE_COUNT - 1);
        float angle = dirAngle - std::numbers::pi_v<float> / 2.f + t * std::numbers::pi_v<float>;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        float ring = SHOCKWAVE_RING_RADIUS * jitter(rng);
        float speed = SHOCKWAVE_PARTICLE_SPEED * jitter(rng);
        float radius = SHOCKWAVE_PARTICLE_RADIUS * jitter(rng);

        Entity entity = coordinator.createEntity();
        coordinator.addComponent(entity, Transform{static_cast<std::int32_t>(cx + cosA * ring),
                                                   static_cast<std::int32_t>(cy + sinA * ring), Z_PLAYER + 1});
        coordinator.addComponent(entity, Particle{radius, color});
        coordinator.addComponent(entity, Velocity{cosA * speed, sinA * speed, SHOCKWAVE_PARTICLE_SLOW});
        coordinator.addComponent(entity, Lifetime{SHOCKWAVE_PARTICLE_DURATION, SHOCKWAVE_PARTICLE_DURATION});
    }
}

/**
 * @brief Spawn a burst of teleport particles centered on a world tile.
 *
 * @param coordinator The coordinator to create entities into
 * @param cx World X center of the tile
 * @param cy World Y center of the tile
 * @param arrival true = arrival, false = depop
 * @param color Team color applied to the particles
 */
void ParticleSystem::spawnTeleport(Coordinator &coordinator, float cx, float cy, bool arrival, sf::Color color)
{
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> angleDist(0.f, 2.f * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> jitter(0.8f, 1.2f);

    for (int i = 0; i < TELEPORT_PARTICLE_COUNT; ++i) {
        float angle = angleDist(rng);
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        float speed = TELEPORT_PARTICLE_SPEED * jitter(rng);
        float ring = TELEPORT_RING_RADIUS * jitter(rng);
        float radius = TELEPORT_PARTICLE_RADIUS * jitter(rng);

        float fx;
        float fy;
        float vx;
        float vy;

        if (arrival) {
            fx = cx + cosA * ring;
            fy = cy + sinA * ring;
            vx = -cosA * speed;
            vy = -sinA * speed;
        } else {
            fx = cx;
            fy = cy;
            vx = cosA * speed;
            vy = sinA * speed;
        }

        Entity entity = coordinator.createEntity();
        coordinator.addComponent(entity,
                                 Transform{static_cast<std::int32_t>(fx), static_cast<std::int32_t>(fy), Z_PLAYER + 1});
        coordinator.addComponent(entity, Particle{radius, color});
        coordinator.addComponent(entity, Velocity{vx, vy, TELEPORT_PARTICLE_SLOW});
        coordinator.addComponent(entity, Lifetime{TELEPORT_PARTICLE_DURATION, TELEPORT_PARTICLE_DURATION});
    }
}
