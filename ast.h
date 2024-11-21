/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */

#ifndef AST_H
#define AST_H

#include "token.h"
#include <stdbool.h>

typedef struct Environment Environment;  // Forward declaration

typedef enum {
    EXPR_IDENTIFIER,
    EXPR_BOOLEAN,
    EXPR_PREFIX,
    EXPR_INFIX
} ExpressionType;

typedef struct Expression Expression;

struct Expression {
    ExpressionType type;
    void* node;  // Points to the specific expression type
    bool (*eval)(Expression* expr, Environment* env);
    char* (*string)(Expression* expr);
    char* (*pretty_print)(Expression* expr, const char* indent);
    void (*free)(Expression* expr);
};

typedef struct {
    Token* token;
    char* value;
} IdentifierExpression;

typedef struct {
    Token* token;
    bool value;
} BooleanExpression;

typedef struct {
    Token* token;
    char* operator;
    Expression* right;
} PrefixExpression;

typedef struct {
    Token* token;
    Expression* left;
    char* operator;
    Expression* right;
} InfixExpression;

Expression* new_identifier(Token* token, const char* value);
Expression* new_boolean(Token* token, bool value);
Expression* new_prefix(Token* token, const char* operator, Expression* right);
Expression* new_infix(Token* token, Expression* left, const char* operator, Expression* right);

#endif