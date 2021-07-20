//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H
#define INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>

const float STANDARD_DEVIATION_FACTOR = 0.3f;

//simulation
const float SIMULATION_STEP_SIZE = 0.01f;
const int CREATURES_PER_GENERATION = 500;
const int NUMBER_OF_GENERATIONS = 100;
const int STEPS_PER_GENERATION = 1500;

const int FALL_DOWN_STEPS = 500;

//body nodes
const float MEAN_PART_SIZE = 3.0f;
const float MEAN_SCALE = 1.0f;
const int MAX_CHILDREN = 2;
const float CHILD_CHANCE = 0.8;
const float CHILD_CHANCE_DECREASE = 0.7;

const float MIN_PART_SIZE = 0.5f;
const float MIN_SCALE = 0.1f;

const float MUTATE_DIMENSION_CHANCE = 0.2;
const float MUTATE_SCALE_CHANCE = 0.2;
const float MUTATE_ANCHOR_CHANCE = 0.2;
const float MUTATE_BODY_CONNECTION_CHANCE = 0.2;
const float MUTATE_REMOVE_BODY_CHILD_CHANCE = 0.1;

//joints
const float MEAN_JOINT_LIMIT = M_PI/4;
const float SPRING_STIFFNESS = 1000;
const float SPRING_DAMPING = 1000;

const float JOINT_LIMIT = M_PI/2;

const float MEAN_SENSOR_WEIGHT = 1.0f;
const float MEAN_EFFECTOR_WEIGHT = 10.0f;

const float EFFECTOR_MAXIMUM_STRENGTH_FACTOR = 0.5;

const float MUTATE_JOINT_CHANCE = 0.2;

//neurons
const int MEAN_LOCAL_NEURONS = 4;
const int MEAN_BRAIN_NEURONS = 8;
const float MEAN_NEURON_WEIGHT = 1.0f;
const float MEAN_NEURON_CONSTANT = 0.0f;
const float MEAN_NEURON_THRESHOLD = 0.0f;

const int BRAINSTEPS_PER_SIMSTEP = 3;

const float MUTATE_OUTPUT_WEIGHT_CHANCE = 0.2;
const float MUTATE_CONSTANT_CHANCE = 0.2;
const float MUTATE_INPUT_WEIGHT_CHANCE = 0.2;
const float MUTATE_CONNECTION_CHANCE = 0.2;
const float MUTATE_NEW_NEURON_CHANCE = 0.2;
const float MUTATE_REMOVE_NEURON_CHANCE = 0.1;
const float MUTATE_THRESHOLD_CHANCE = 0.2;
const float MUTATE_SINE_CHANCE = 0.2;

#endif //INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H
