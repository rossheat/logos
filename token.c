/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */
   
#include "token.h"
#include <stdlib.h>
#include <string.h>

Token* token_new(TokenType type, const char* literal) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    token->literal = literal ? strdup(literal) : NULL;
    return token;
}

void token_free(Token* token) {
    if (token) {
        free(token->literal);
        free(token);
    }
}