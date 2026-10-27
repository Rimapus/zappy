/*
** EPITECH PROJECT, 2026
** gui/src/Systems/AnimatorSystem/AnimatorSystem
** File description:
** AnimatorSystem source file
*/

#include "AnimatorSystem.hpp"
#include "../../Components/Animator.hpp"
#include "../../Components/Sprite.hpp"
#include "../../ECS/Coordinator/Coordinator.hpp"
#include "../../Resources/TextureManager.hpp"

/**
 * @brief Swap the entity's sprite texture to the one a clip lives on.
 *
 * Player animations are spread over several spritesheets (idle, walk, ...),
 * so starting a clip must point the sprite at the matching texture before
 * its frame rects make sense.
 *
 * @param coordinator      The coordinator to access components/resources
 * @param entity           The entity whose sprite is updated
 * @param clip             The clip about to be played
 */
static void applyClipTexture(Coordinator &coordinator, Entity entity, const AnimationClip &clip)
{
    Sprite &sprite = coordinator.getComponent<Sprite>(entity);
    TextureManager &textures = coordinator.getResource<TextureManager>();

    if (clip._texture.empty() == true || sprite._sprite.has_value() == false) {
        return;
    }
    if (textures.has(clip._texture) == false) {
        return;
    }
    sprite._sprite->setTexture(textures.get(clip._texture), false);
}

/**
 * @brief Play one animation (loops if the clip's _loop is true).
 *
 * @param coordinator      The coordinator to access components
 * @param entity           The entity to play the animation on
 * @param name             The name of the animation
 */
void AnimatorSystem::play(Coordinator &coordinator, Entity entity, const std::string &name)
{
    Animator &animator = coordinator.getComponent<Animator>(entity);
    auto it = animator._clips.find(name);

    if (it == animator._clips.end()) {
        return;
    }
    if (animator._current == name) {
        return;
    }
    animator._current = name;
    animator._queued.clear();
    animator._frameIndex = 0;
    animator._elapsed = 0.f;
    animator._playing = true;
    applyClipTexture(coordinator, entity, it->second);
}

/**
 * @brief Play one animation once, then switch to a looping one.
 *
 * @param coordinator      The coordinator to access components
 * @param entity           The entity to play the animation on
 * @param name             Animation played a single time (clip must be _loop = false)
 * @param loop             Animation looped once name has finished
 */
void AnimatorSystem::playOnce(Coordinator &coordinator, Entity entity, const std::string &name, const std::string &loop)
{
    Animator &animator = coordinator.getComponent<Animator>(entity);
    auto it = animator._clips.find(name);

    if (it == animator._clips.end()) {
        return;
    }
    if (animator._current == name) {
        return;
    }
    animator._current = name;
    animator._queued = loop;
    animator._frameIndex = 0;
    animator._elapsed = 0.f;
    animator._playing = true;
    applyClipTexture(coordinator, entity, it->second);
}

/**
 * @brief Advance every entity's animation by dt and push the current frame.
 *
 * @param coordinator      The coordinator to access components
 * @param dt               Elapsed time since the last frame (seconds)
 */
void AnimatorSystem::update(Coordinator &coordinator, float dt)
{
    for (Entity const &entity : _entities) {

        Animator &animator = coordinator.getComponent<Animator>(entity);
        Sprite &sprite = coordinator.getComponent<Sprite>(entity);
        auto it = animator._clips.find(animator._current);

        if (animator._playing == false || it == animator._clips.end()) {
            continue;
        }
        AnimationClip &clip = it->second;

        if (clip._frames.empty()) {
            continue;
        }
        animator._elapsed += dt;
        while (animator._elapsed >= clip._frameDuration) {
            animator._elapsed -= clip._frameDuration;
            animator._frameIndex++;
            if (animator._frameIndex < static_cast<int>(clip._frames.size())) {
                continue;
            }
            if (clip._loop == true) {
                animator._frameIndex = 0;
            } else if (animator._queued.empty() == false) {
                play(coordinator, entity, animator._queued);
                break;
            } else {
                animator._frameIndex = static_cast<int>(clip._frames.size()) - 1;
                animator._playing = false;
                break;
            }
        }
        if (sprite._sprite.has_value() == true) {
            sprite._sprite->setTextureRect(clip._frames[animator._frameIndex]);
        }
    }
}
