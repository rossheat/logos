/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <stdbool.h>

enum Precedence {
    PREC_LOWEST,
    PREC_EQUALS,    // <->
    PREC_IMPLIES,   // ->
    PREC_OR_XOR,    // | ^
    PREC_AND,       // &
    PREC_PREFIX     // ~
};

typedef struct Parser {
    Lexer* lexer;
    Token* cur_token;
    Token* peek_token;
    char** errors;
    int error_count;
    int error_capacity;
} Parser;

Parser* parser_new(Lexer* l);
void parser_free(Parser* p);
Expression* parser_parse_expression(Parser* p, int precedence);
void parser_next_token(Parser* p);
bool parser_expect_peek(Parser* p, TokenType type);
void parser_add_error(Parser* p, const char* msg);

#endif