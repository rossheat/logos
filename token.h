/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */

#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    T_ILLEGAL,
    T_EOF,
    T_IDENT,
    T_TRUE,
    T_FALSE,
    T_SET,
    T_LPAREN,
    T_RPAREN,
    T_NOT,
    T_AND,
    T_OR,
    T_XOR,
    T_IMPLIES,
    T_IFF
} TokenType;

typedef struct {
    TokenType type;
    char* literal;
} Token;

Token* token_new(TokenType type, const char* literal);
void token_free(Token* token);

#endif