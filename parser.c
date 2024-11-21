/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */

#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_ERROR_CAPACITY 16

static int get_precedence(TokenType type) {
    switch (type) {
        case T_IFF: return PREC_EQUALS;
        case T_IMPLIES: return PREC_IMPLIES;
        case T_OR:
        case T_XOR: return PREC_OR_XOR;
        case T_AND: return PREC_AND;
        default: return PREC_LOWEST;
    }
}

Parser* parser_new(Lexer* l) {
    Parser* p = malloc(sizeof(Parser));
    p->lexer = l;
    p->errors = malloc(sizeof(char*) * INITIAL_ERROR_CAPACITY);
    p->error_count = 0;
    p->error_capacity = INITIAL_ERROR_CAPACITY;
    
    parser_next_token(p);
    parser_next_token(p);
    
    return p;
}

void parser_free(Parser* p) {
    if (!p) return;
    
    token_free(p->cur_token);
    token_free(p->peek_token);
    
    for (int i = 0; i < p->error_count; i++) {
        free(p->errors[i]);
    }
    free(p->errors);
    
    // Note: Don't free lexer here as it's owned by the caller
    free(p);
}

void parser_next_token(Parser* p) {
    token_free(p->cur_token);
    p->cur_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

void parser_add_error(Parser* p, const char* msg) {
    if (p->error_count >= p->error_capacity) {
        p->error_capacity *= 2;
        p->errors = realloc(p->errors, sizeof(char*) * p->error_capacity);
    }
    p->errors[p->error_count++] = strdup(msg);
}

bool parser_expect_peek(Parser* p, TokenType type) {
    if (p->peek_token->type == type) {
        parser_next_token(p);
        return true;
    }
    
    char error[100];
    snprintf(error, sizeof(error), "expected next token to be %d, got %d instead",
             type, p->peek_token->type);
    parser_add_error(p, error);
    return false;
}

static Expression* parse_identifier(Parser* p) {
    return new_identifier(token_new(p->cur_token->type, p->cur_token->literal),
                         p->cur_token->literal);
}

static Expression* parse_boolean(Parser* p) {
    return new_boolean(token_new(p->cur_token->type, p->cur_token->literal),
                      p->cur_token->type == T_TRUE);
}

static Expression* parse_prefix_expression(Parser* p) {
    Token* token = token_new(p->cur_token->type, p->cur_token->literal);
    char* operator = strdup(p->cur_token->literal);
    
    parser_next_token(p);
    Expression* right = parser_parse_expression(p, PREC_PREFIX);
    
    return new_prefix(token, operator, right);
}

static Expression* parse_infix_expression(Parser* p, Expression* left) {
    Token* token = token_new(p->cur_token->type, p->cur_token->literal);
    char* operator = strdup(p->cur_token->literal);
    int precedence = get_precedence(p->cur_token->type);
    
    parser_next_token(p);
    Expression* right = parser_parse_expression(p, precedence);
    
    return new_infix(token, left, operator, right);
}

static Expression* parse_grouped_expression(Parser* p) {
    parser_next_token(p);
    
    Expression* exp = parser_parse_expression(p, PREC_LOWEST);
    
    if (!parser_expect_peek(p, T_RPAREN)) {
        exp->free(exp);
        return NULL;
    }
    
    return exp;
}

Expression* parser_parse_expression(Parser* p, int precedence) {
    Expression* left = NULL;
    
    // Prefix
    switch (p->cur_token->type) {
        case T_IDENT:
            left = parse_identifier(p);
            break;
        case T_TRUE:
        case T_FALSE:
            left = parse_boolean(p);
            break;
        case T_LPAREN:
            left = parse_grouped_expression(p);
            break;
        case T_NOT:
            left = parse_prefix_expression(p);
            break;
        default:
            {
                char error[100];
                snprintf(error, sizeof(error), "no prefix parse function for %d found",
                         p->cur_token->type);
                parser_add_error(p, error);
                return NULL;
            }
    }
    
    // Infix
    while (precedence < get_precedence(p->peek_token->type)) {
        switch (p->peek_token->type) {
            case T_AND:
            case T_OR:
            case T_XOR:
            case T_IMPLIES:
            case T_IFF:
                parser_next_token(p);
                left = parse_infix_expression(p, left);
                break;
            default:
                return left;
        }
    }
    
    return left;
}