/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */
   
#include "environment.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

Environment* environment_new(void) {
    Environment* env = malloc(sizeof(Environment));
    env->store = malloc(sizeof(*env->store) * INITIAL_CAPACITY);
    env->capacity = INITIAL_CAPACITY;
    env->size = 0;
    
    env->settings = malloc(sizeof(*env->settings) * INITIAL_CAPACITY);
    env->settings_capacity = INITIAL_CAPACITY;
    env->settings_size = 0;
    
    return env;
}

void environment_free(Environment* env) {
    if (!env) return;
    
    for (int i = 0; i < env->size; i++) {
        free(env->store[i].key);
    }
    free(env->store);
    
    for (int i = 0; i < env->settings_size; i++) {
        free(env->settings[i].key);
    }
    free(env->settings);
    
    free(env);
}

static void expand_if_needed(Environment* env) {
    if (env->size >= env->capacity) {
        env->capacity *= 2;
        env->store = realloc(env->store, sizeof(*env->store) * env->capacity);
    }
}

static void expand_settings_if_needed(Environment* env) {
    if (env->settings_size >= env->settings_capacity) {
        env->settings_capacity *= 2;
        env->settings = realloc(env->settings, sizeof(*env->settings) * env->settings_capacity);
    }
}

void environment_set(Environment* env, const char* name, bool value) {
    for (int i = 0; i < env->size; i++) {
        if (strcmp(env->store[i].key, name) == 0) {
            env->store[i].value = value;
            return;
        }
    }
    
    expand_if_needed(env);
    env->store[env->size].key = strdup(name);
    env->store[env->size].value = value;
    env->size++;
}

bool environment_get(Environment* env, const char* name, bool* value) {
    for (int i = 0; i < env->size; i++) {
        if (strcmp(env->store[i].key, name) == 0) {
            *value = env->store[i].value;
            return true;
        }
    }
    return false;
}

void environment_set_setting(Environment* env, const char* name, bool value) {
    for (int i = 0; i < env->settings_size; i++) {
        if (strcmp(env->settings[i].key, name) == 0) {
            env->settings[i].value = value;
            return;
        }
    }
    
    expand_settings_if_needed(env);
    env->settings[env->settings_size].key = strdup(name);
    env->settings[env->settings_size].value = value;
    env->settings_size++;
}

bool environment_get_setting(Environment* env, const char* name) {
    for (int i = 0; i < env->settings_size; i++) {
        if (strcmp(env->settings[i].key, name) == 0) {
            return env->settings[i].value;
        }
    }
    return false;
}