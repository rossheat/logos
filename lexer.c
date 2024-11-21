/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */
   
#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Lexer* lexer_new(const char* input) {
    Lexer* l = malloc(sizeof(Lexer));
    l->input = strdup(input);
    l->position = 0;
    l->read_position = 0;
    l->ch = 0;
    lexer_read_char(l);
    return l;
}

void lexer_free(Lexer* l) {
    if (l) {
        free(l->input);
        free(l);
    }
}

void lexer_read_char(Lexer* l) {
    if (l->read_position >= (int)strlen(l->input)) {
        l->ch = 0;
    } else {
        l->ch = l->input[l->read_position];
    }
    l->position = l->read_position;
    l->read_position += 1;
}

char lexer_peek_char(Lexer* l) {
    if (l->read_position >= (int)strlen(l->input)) {
        return 0;
    }
    return l->input[l->read_position];
}

void lexer_skip_whitespace(Lexer* l) {
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
        lexer_read_char(l);
    }
}

int is_letter(char ch) {
    return isalpha(ch);
}

char* lexer_read_identifier(Lexer* l) {
    int start_pos = l->position;
    while (is_letter(l->ch)) {
        lexer_read_char(l);
    }
    int length = l->position - start_pos;
    char* identifier = malloc(length + 1);
    strncpy(identifier, &l->input[start_pos], length);
    identifier[length] = '\0';
    return identifier;
}

Token* lexer_next_token(Lexer* l) {
    Token* tok;
    lexer_skip_whitespace(l);

    char ch_str[2] = {l->ch, '\0'};

    switch (l->ch) {
        case '(':
            tok = token_new(T_LPAREN, ch_str);
            break;
        case ')':
            tok = token_new(T_RPAREN, ch_str);
            break;
        case '~':
            tok = token_new(T_NOT, ch_str);
            break;
        case '&':
            tok = token_new(T_AND, ch_str);
            break;
        case '|':
            tok = token_new(T_OR, ch_str);
            break;
        case '^':
            tok = token_new(T_XOR, ch_str);
            break;
        case '-':
            if (lexer_peek_char(l) == '>') {
                lexer_read_char(l);
                tok = token_new(T_IMPLIES, "->");
            } else {
                tok = token_new(T_ILLEGAL, ch_str);
            }
            break;
        case '<':
            if (lexer_peek_char(l) == '-') {
                lexer_read_char(l);
                if (lexer_peek_char(l) == '>') {
                    lexer_read_char(l);
                    tok = token_new(T_IFF, "<->");
                } else {
                    tok = token_new(T_ILLEGAL, "<-");
                }
            } else {
                tok = token_new(T_ILLEGAL, ch_str);
            }
            break;
        case 0:
            tok = token_new(T_EOF, "");
            break;
        default:
            if (is_letter(l->ch)) {
                char* ident = lexer_read_identifier(l);
                if (strcmp(ident, "SET") == 0) {
                    tok = token_new(T_SET, ident);
                } else if (strcmp(ident, "true") == 0) {
                    tok = token_new(T_TRUE, ident);
                } else if (strcmp(ident, "false") == 0) {
                    tok = token_new(T_FALSE, ident);
                } else {
                    tok = token_new(T_IDENT, ident);
                }
                free(ident);
                return tok;
            } else {
                tok = token_new(T_ILLEGAL, ch_str);
            }
    }

    lexer_read_char(l);
    return tok;
}