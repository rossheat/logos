/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */

#include "repl.h"
#include "lexer.h"
#include "parser.h"
#include "environment.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define OUTPUT_AST "OUTPUT_AST"

static bool parse_bool(const char* str) {
    return strcmp(str, "true") == 0;
}

static void handle_set_command(char* line, Environment* env) {
    char var[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
    
    if (sscanf(line, "SET %s %s", var, value) != 2) {
        printf("Invalid SET command. Use: SET <var> true/false\n");
        return;
    }
    
    if (strcmp(value, "true") != 0 && strcmp(value, "false") != 0) {
        printf("Invalid value. Use true or false\n");
        return;
    }
    
    if (strcmp(var, OUTPUT_AST) == 0) {
        environment_set_setting(env, OUTPUT_AST, parse_bool(value));
        printf("Set %s to %s\n", var, value);
        return;
    }
    
    environment_set(env, var, parse_bool(value));
    printf("Set %s to %s\n", var, value);
}

void start_repl(void) {
    Environment* env = environment_new();
    char line[MAX_LINE_LENGTH];
    
    printf("Propositional Logic REPL\n");
    printf("Use SET <var> true/false to define variables\n");
    printf("Use SET OUTPUT_AST true/false to toggle AST output\n");
    printf("Use expressions using ~(NOT), &(AND), |(OR), ^(XOR), ->(IMPLIES), <->(IFF)\n");
    printf(">> ");
    
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        
        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
            break;
        }
        
        if (strncmp(line, "SET", 3) == 0) {
            handle_set_command(line, env);
            printf(">> ");
            continue;
        }
        
        Lexer* l = lexer_new(line);
        Parser* p = parser_new(l);
        Expression* expression = parser_parse_expression(p, PREC_LOWEST);
        
        if (p->error_count > 0) {
            for (int i = 0; i < p->error_count; i++) {
                printf("Error: %s\n", p->errors[i]);
            }
            parser_free(p);
            lexer_free(l);
            printf(">> ");
            continue;
        }
        
        if (expression) {
            if (environment_get_setting(env, OUTPUT_AST)) {
                printf("AST:\n");
                char* ast = expression->pretty_print(expression, "");
                printf("%s\n", ast);
                free(ast);
            }
            
            bool result;
            bool has_error = false;
            
            // Simple error handling using setjmp/longjmp could be added here
            // For now, we'll just try to evaluate and catch any undefined variables
            result = expression->eval(expression, env);
            
            if (!has_error) {
                printf("Result: %s\n", result ? "true" : "false");
            }
            
            expression->free(expression);
        }
        
        parser_free(p);
        lexer_free(l);
        printf(">> ");
    }
    
    environment_free(env);
}