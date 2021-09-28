//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H
#define INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float STANDARD_DEVIATION_FACTOR = 0.5f;
const float STANDARD_MUTATION_CHANCE = 0.04f;

//evolution
const float EVOLUTION_MIN_SCORE = 0.99;
const int EVOLUTION_MAX_PARENTS = 10;
const int AMOUNT_STEPS_DELTA = 100;
const float MAX_ALLOWED_HEIGHT = 10;

//simulation
const float SIMULATION_STEP_SIZE = 0.01f;
const int CREATURES_PER_GENERATION = 500;
const int NUMBER_OF_GENERATIONS = 100;
const int STEPS_PER_GENERATION = 2500;

const int FALL_DOWN_STEPS = 500;
const float ROOT_STARTING_HEIGHT = 5;

//body nodes
const float CREATURE_DENSITY = 10.0f;
const float MEAN_PART_SIZE = 3.0f;
const int MAX_CHILDREN = 2;
const float CHILD_CHANCE = 0.75f;

const float MIN_PART_SIZE = 1.0f;
const float MIN_SCALE = 0.1f;
const float MAX_ROTATION = 180;

const float MUTATE_REFLECTION_FLAG_CHANCE = 0.1f;

//joints
const float MEAN_JOINT_LIMIT = M_PI/2;
const float SPRING_STIFFNESS = 10.0f;
const float SPRING_DAMPING = 0.1f;

const float JOINT_LIMIT = M_PI/2;
const float MAX_JOINT_VELOCITY = 4.0f;

const int NUMBER_SAVED_EFFECTOR_VALUES = 10;

const float EFFECTOR_MAXIMUM_STRENGTH_FACTOR = 0.1f;

//neurons
const int MEAN_LOCAL_NEURONS = 4;
const int MEAN_BRAIN_NEURONS = 8;

const int BRAINSTEPS_PER_SIMSTEP = 2;

const glm::vec4 COLOR_GREEN(0.0f, 0.8f, 0.3f, 1.0f);
const glm::vec4 COLOR_RED(0.8f, 0.2f, 0.3f, 1.0f);
const glm::vec4 COLOR_PLANE(0.0f, 0.5f, 0.2f, 1.0f);
const glm::vec4 COLOR_CLEAR(0.0f, 0.7f, 0.9f, 1.0f);

#endif //INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H
