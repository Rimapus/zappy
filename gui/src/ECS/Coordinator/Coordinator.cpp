/*
** EPITECH PROJECT, 2026
** gui/src/ECS/Coordinator/Coordinator
** File description:
** Coordinator source file
*/

#include "Coordinator.hpp"
#include "../../Components/Animator.hpp"
#include "../../Components/Background.hpp"
#include "../../Components/Egg.hpp"
#include "../../Components/Glow.hpp"
#include "../../Components/Inventory.hpp"
#include "../../Components/Levitate.hpp"
#include "../../Components/Lifetime.hpp"
#include "../../Components/Movement.hpp"
#include "../../Components/Offsets.hpp"
#include "../../Components/Particle.hpp"
#include "../../Components/Player.hpp"
#include "../../Components/Sprite.hpp"
#include "../../Components/Tile.hpp"
#include "../../Components/TileAnchor.hpp"
#include "../../Components/TileMap.hpp"
#include "../../Components/Velocity.hpp"

/**
 * @brief Coordinator constructor.
 *
 * @return Coordinator instance.
 */
Coordinator::Coordinator(void)
    : _componentManager(std::make_unique<ComponentManager>()),
      _entityManager(std::make_unique<EntityManager>()),
      _systemManager(std::make_unique<SystemManager>())
{
    // Register every components here:
    registerComponent<Transform>();
    registerComponent<Sprite>();
    registerComponent<Animator>();
    registerComponent<TileMap>();
    registerComponent<Background>();
    registerComponent<Player>();
    registerComponent<Egg>();
    registerComponent<Lifetime>();
    registerComponent<Inventory>();
    registerComponent<Movement>();
    registerComponent<Glow>();
    registerComponent<Tile>();
    registerComponent<Offsets>();
    registerComponent<TileAnchor>();
    registerComponent<Levitate>();
    registerComponent<Particle>();
    registerComponent<Velocity>();
}

/**
 * @brief Create an entity.
 *
 * @return The new entity.
 */
Entity Coordinator::createEntity(void)
{
    return _entityManager->createEntity();
}

/**
 * @brief Destroy an entity.
 *
 * @param entity	 The entity to destroy
 */
void Coordinator::destroyEntity(Entity entity)
{
    _entityManager->destroyEntity(entity);
    _componentManager->entityDestroyed(entity);
    _systemManager->entityDestroyed(entity);
}
