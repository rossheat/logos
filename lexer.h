/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */
   
#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct {
    char* input;
    int position;
    int read_position;
    char ch;
} Lexer;

Lexer* lexer_new(const char* input);
void lexer_free(Lexer* l);
Token* lexer_next_token(Lexer* l);
void lexer_read_char(Lexer* l);
char lexer_peek_char(Lexer* l);
void lexer_skip_whitespace(Lexer* l);
char* lexer_read_identifier(Lexer* l);
int is_letter(char ch);

#endif