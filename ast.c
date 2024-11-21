/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */

#include "ast.h"
#include "environment.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* string_concat(const char* s1, const char* s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char* result = malloc(len1 + len2 + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

bool eval_identifier(Expression* expr, Environment* env) {
    IdentifierExpression* ident = (IdentifierExpression*)expr->node;
    bool value;
    if (!environment_get(env, ident->value, &value)) {
        fprintf(stderr, "undefined variable: %s\n", ident->value);
        exit(1);
    }
    return value;
}

bool eval_boolean(Expression* expr, Environment* env) {
    BooleanExpression* boolean = (BooleanExpression*)expr->node;
    return boolean->value;
}

bool eval_prefix(Expression* expr, Environment* env) {
    PrefixExpression* prefix = (PrefixExpression*)expr->node;
    bool right = prefix->right->eval(prefix->right, env);
    return !right;  // Only NOT operator is supported as prefix
}

bool eval_infix(Expression* expr, Environment* env) {
    InfixExpression* infix = (InfixExpression*)expr->node;
    bool left = infix->left->eval(infix->left, env);
    bool right = infix->right->eval(infix->right, env);

    if (strcmp(infix->operator, "&") == 0) return left && right;
    if (strcmp(infix->operator, "|") == 0) return left || right;
    if (strcmp(infix->operator, "^") == 0) return left != right;
    if (strcmp(infix->operator, "->") == 0) return !left || right;
    if (strcmp(infix->operator, "<->") == 0) return left == right;

    fprintf(stderr, "unknown operator: %s\n", infix->operator);
    exit(1);
}

char* string_identifier(Expression* expr) {
    IdentifierExpression* ident = (IdentifierExpression*)expr->node;
    return strdup(ident->value);
}

char* string_boolean(Expression* expr) {
    BooleanExpression* boolean = (BooleanExpression*)expr->node;
    return strdup(boolean->value ? "true" : "false");
}

char* string_prefix(Expression* expr) {
    PrefixExpression* prefix = (PrefixExpression*)expr->node;
    char* right_str = prefix->right->string(prefix->right);
    char* result = malloc(strlen(prefix->operator) + strlen(right_str) + 3);
    sprintf(result, "(%s%s)", prefix->operator, right_str);
    free(right_str);
    return result;
}

char* string_infix(Expression* expr) {
    InfixExpression* infix = (InfixExpression*)expr->node;
    char* left_str = infix->left->string(infix->left);
    char* right_str = infix->right->string(infix->right);
    char* result = malloc(strlen(left_str) + strlen(infix->operator) + strlen(right_str) + 5);
    sprintf(result, "(%s %s %s)", left_str, infix->operator, right_str);
    free(left_str);
    free(right_str);
    return result;
}

// Pretty print functions
char* pretty_print_identifier(Expression* expr, const char* indent) {
    IdentifierExpression* ident = (IdentifierExpression*)expr->node;
    char* result = malloc(strlen(indent) + strlen(ident->value) + 20);
    sprintf(result, "%sIdentifier(%s)", indent, ident->value);
    return result;
}

char* pretty_print_boolean(Expression* expr, const char* indent) {
    BooleanExpression* boolean = (BooleanExpression*)expr->node;
    char* result = malloc(strlen(indent) + 20);
    sprintf(result, "%sBoolean(%s)", indent, boolean->value ? "true" : "false");
    return result;
}

char* pretty_print_prefix(Expression* expr, const char* indent) {
    PrefixExpression* prefix = (PrefixExpression*)expr->node;
    char* new_indent = string_concat(indent, "    ");
    char* right_str = prefix->right->pretty_print(prefix->right, new_indent);
    
    char* result = malloc(strlen(indent) + strlen(prefix->operator) + strlen(right_str) + 100);
    sprintf(result, "%sPrefix[\n%s  Operator: %s\n%s  Right:\n%s\n%s]",
            indent, indent, prefix->operator, indent, right_str, indent);
            
    free(new_indent);
    free(right_str);
    return result;
}

char* pretty_print_infix(Expression* expr, const char* indent) {
    InfixExpression* infix = (InfixExpression*)expr->node;
    char* new_indent = string_concat(indent, "    ");
    char* left_str = infix->left->pretty_print(infix->left, new_indent);
    char* right_str = infix->right->pretty_print(infix->right, new_indent);
    
    char* result = malloc(strlen(indent) + strlen(infix->operator) + 
                         strlen(left_str) + strlen(right_str) + 100);
    sprintf(result, "%sInfix[\n%s  Operator: %s\n%s  Left:\n%s\n%s  Right:\n%s\n%s]",
            indent, indent, infix->operator, indent, left_str, indent, right_str, indent);
            
    free(new_indent);
    free(left_str);
    free(right_str);
    return result;
}

void free_identifier(Expression* expr) {
    IdentifierExpression* ident = (IdentifierExpression*)expr->node;
    token_free(ident->token);
    free(ident->value);
    free(ident);
    free(expr);
}

void free_boolean(Expression* expr) {
    BooleanExpression* boolean = (BooleanExpression*)expr->node;
    token_free(boolean->token);
    free(boolean);
    free(expr);
}

void free_prefix(Expression* expr) {
    PrefixExpression* prefix = (PrefixExpression*)expr->node;
    token_free(prefix->token);
    free(prefix->operator);
    prefix->right->free(prefix->right);
    free(prefix);
    free(expr);
}

void free_infix(Expression* expr) {
    InfixExpression* infix = (InfixExpression*)expr->node;
    token_free(infix->token);
    infix->left->free(infix->left);
    free(infix->operator);
    infix->right->free(infix->right);
    free(infix);
    free(expr);
}

Expression* new_identifier(Token* token, const char* value) {
    Expression* expr = malloc(sizeof(Expression));
    IdentifierExpression* ident = malloc(sizeof(IdentifierExpression));
    
    ident->token = token;
    ident->value = strdup(value);
    
    expr->type = EXPR_IDENTIFIER;
    expr->node = ident;
    expr->eval = eval_identifier;
    expr->string = string_identifier;
    expr->pretty_print = pretty_print_identifier;
    expr->free = free_identifier;
    
    return expr;
}

Expression* new_boolean(Token* token, bool value) {
    Expression* expr = malloc(sizeof(Expression));
    BooleanExpression* boolean = malloc(sizeof(BooleanExpression));
    
    boolean->token = token;
    boolean->value = value;
    
    expr->type = EXPR_BOOLEAN;
    expr->node = boolean;
    expr->eval = eval_boolean;
    expr->string = string_boolean;
    expr->pretty_print = pretty_print_boolean;
    expr->free = free_boolean;
    
    return expr;
}

Expression* new_prefix(Token* token, const char* operator, Expression* right) {
    Expression* expr = malloc(sizeof(Expression));
    PrefixExpression* prefix = malloc(sizeof(PrefixExpression));
    
    prefix->token = token;
    prefix->operator = strdup(operator);
    prefix->right = right;
    
    expr->type = EXPR_PREFIX;
    expr->node = prefix;
    expr->eval = eval_prefix;
    expr->string = string_prefix;
    expr->pretty_print = pretty_print_prefix;
    expr->free = free_prefix;
    
    return expr;
}

Expression* new_infix(Token* token, Expression* left, const char* operator, Expression* right) {
    Expression* expr = malloc(sizeof(Expression));
    InfixExpression* infix = malloc(sizeof(InfixExpression));
    
    infix->token = token;
    infix->left = left;
    infix->operator = strdup(operator);
    infix->right = right;
    
    expr->type = EXPR_INFIX;
    expr->node = infix;
    expr->eval = eval_infix;
    expr->string = string_infix;
    expr->pretty_print = pretty_print_infix;
    expr->free = free_infix;
    
    return expr;
}