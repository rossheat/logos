/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */
   
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdbool.h>

typedef struct Environment {
    struct {
        char* key;
        bool value;
    } *store;
    int capacity;
    int size;
    
    struct {
        char* key;
        bool value;
    } *settings;
    int settings_capacity;
    int settings_size;
} Environment;

Environment* environment_new(void);
void environment_free(Environment* env);
void environment_set(Environment* env, const char* name, bool value);
bool environment_get(Environment* env, const char* name, bool* value);
void environment_set_setting(Environment* env, const char* name, bool value);
bool environment_get_setting(Environment* env, const char* name);

#endif
