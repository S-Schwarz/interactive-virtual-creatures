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
const int CREATURES_PER_GENERATION = 100;
const int NUMBER_OF_GENERATIONS = 50;
const int STEPS_PER_GENERATION = 1500;

//body nodes
const float MEAN_PART_SIZE = 1.0f;
const float MEAN_SCALE = 1.0f;
const int MAX_CHILDREN = 3;
const float CHILD_CHANCE = 0.6;
const float CHILD_CHANCE_DECREASE = 0.4;

//joints
const float MEAN_JOINT_LIMIT = M_PI/8;
const float SPRING_STIFFNESS = 1000;
const float SPRING_DAMPING = 1000;

const float JOINT_SWING_LIMIT = M_PI/4;
const float JOINT_TWIST_LIMIT = M_PI/4;

const float MEAN_SENSOR_WEIGHT = 1.0f;
const float MEAN_EFFECTOR_WEIGHT = 10.0f;

//neurons
const int MEAN_LOCAL_NEURONS = 5;
const int MEAN_BRAIN_NEURONS = 10;
const float MEAN_NEURON_WEIGHT = 1.0f;
const float MEAN_NEURON_CONSTANT = 10.0f;

const int BRAINSTEPS_PER_SIMSTEP = 3;

#endif //INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H
