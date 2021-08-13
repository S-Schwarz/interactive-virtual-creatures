//
// Created by st on 7/7/21.
//

#ifndef INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H
#define INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>

const float STANDARD_DEVIATION_FACTOR = 0.3f;
const float STANDARD_MUTATION_CHANCE = 0.3f;

//evolution
const float EVOLUTION_MIN_SCORE = 0.7;
const int AMOUNT_STEPS_DELTA = 100;
const float MAX_ALLOWED_HEIGHT = 20;

//simulation
const float SIMULATION_STEP_SIZE = 0.01f;
const int CREATURES_PER_GENERATION = 500;
const int NUMBER_OF_GENERATIONS = 50;
const int STEPS_PER_GENERATION = 1500;

const int FALL_DOWN_STEPS = 1000;
const float ROOT_STARTING_HEIGHT = 10;

//body nodes
const float CREATURE_DENSITY = 10.0f;
const float MEAN_PART_SIZE = 4.0f;
const float MEAN_SCALE = 1.0f;
const int MAX_CHILDREN = 2;
const float CHILD_CHANCE = 1.0;
const float CHILD_CHANCE_DECREASE = 0.5;

const float MIN_PART_SIZE = 2.0f;
const float MIN_SCALE = 0.1f;

const float MUTATE_DIMENSION_CHANCE = STANDARD_MUTATION_CHANCE;
const float MUTATE_SCALE_CHANCE = STANDARD_MUTATION_CHANCE;
const float MUTATE_ANCHOR_CHANCE = STANDARD_MUTATION_CHANCE;
const float MUTATE_BODY_CONNECTION_CHANCE = 0.4;
const float MUTATE_REMOVE_BODY_CHILD_CHANCE = 0.1;

//joints
const float MEAN_JOINT_LIMIT = M_PI/4;
const float SPRING_STIFFNESS = 10.0f;
const float SPRING_DAMPING = 1.0f;

const float JOINT_LIMIT = M_PI/2;
const float MAX_JOINT_VELOCITY = 4.0f;

const float MEAN_SENSOR_WEIGHT = 1.0f;
const float MEAN_EFFECTOR_WEIGHT = 1.0f;

const float EFFECTOR_MAXIMUM_STRENGTH_FACTOR = 2.5f;

const float MUTATE_JOINT_CHANCE = STANDARD_MUTATION_CHANCE;

//neurons
const int MEAN_LOCAL_NEURONS = 6;
const int MEAN_BRAIN_NEURONS = 12;
const float MEAN_NEURON_WEIGHT = 1.0f;
const float MEAN_NEURON_CONSTANT = 1.0f;
const float MEAN_NEURON_THRESHOLD = 0.0f;

const int BRAINSTEPS_PER_SIMSTEP = 2;

const float MUTATE_OUTPUT_WEIGHT_CHANCE = STANDARD_MUTATION_CHANCE;
const float MUTATE_CONSTANT_CHANCE = STANDARD_MUTATION_CHANCE;
const float MUTATE_INPUT_WEIGHT_CHANCE = STANDARD_MUTATION_CHANCE;
const float MUTATE_CONNECTION_CHANCE = 0.4;
const float MUTATE_NEW_NEURON_CHANCE = 0.6;
const float MUTATE_REMOVE_NEURON_CHANCE = 0.1;
const float MUTATE_THRESHOLD_CHANCE = STANDARD_MUTATION_CHANCE;
const float MUTATE_SINE_CHANCE = STANDARD_MUTATION_CHANCE;
const float MUTATE_OSCI_CHANCE = STANDARD_MUTATION_CHANCE;

const float MUTATE_SIGN_CHANCE = STANDARD_MUTATION_CHANCE;

#endif //INTERACTIVE_VIRTUAL_CREATURES_CONSTANTS_H
