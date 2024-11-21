/* Copyright (C) 2024 Ross Heaton
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version. */
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "environment.h"

typedef struct {
    bool P, Q, R, S;
    const char* expr;
    bool expected;
    const char* desc;
} TestCase;

static void run_test_case(TestCase* tc) {
    Environment* env = environment_new();
    environment_set(env, "P", tc->P);
    environment_set(env, "Q", tc->Q);
    environment_set(env, "R", tc->R);
    environment_set(env, "S", tc->S);
    
    Lexer* l = lexer_new(tc->expr);
    Parser* p = parser_new(l);
    Expression* expr = parser_parse_expression(p, PREC_LOWEST);
    
    if (p->error_count > 0) {
        printf("FAIL: %s\n", tc->desc);
        printf("Parser errors:\n");
        for (int i = 0; i < p->error_count; i++) {
            printf("  %s\n", p->errors[i]);
        }
        goto cleanup;
    }
    
    bool result = expr->eval(expr, env);
    
    if (result != tc->expected) {
        printf("FAIL: %s\n", tc->desc);
        printf("Expected: %s\n", tc->expected ? "true" : "false");
        printf("Got: %s\n", result ? "true" : "false");
        goto cleanup;
    }
    
    printf("PASS: %s\n", tc->desc);
    
cleanup:
    expr->free(expr);
    parser_free(p);
    lexer_free(l);
    environment_free(env);
}

void run_tests(void) {
    TestCase test_cases[] = {
        {true, false, true, false, "P", true, "Simple variable P"},
        {true, false, true, false, "~P", false, "Simple NOT operation"},
        
        {true, true, false, false, "P & Q", true, "Basic AND with true & true"},
        {true, false, false, false, "P & Q", false, "Basic AND with true & false"},
        
        {true, false, false, false, "P | Q", true, "Basic OR with true | false"},
        {false, false, false, false, "P | Q", false, "Basic OR with false | false"},
        
        {true, true, false, false, "P ^ Q", false, "Basic XOR with true ^ true"},
        {true, false, false, false, "P ^ Q", true, "Basic XOR with true ^ false"},
        
        {true, false, false, false, "P -> Q", false, "Basic implication with true -> false"},
        {false, true, false, false, "P -> Q", true, "Basic implication with false -> true"},
        
        {true, true, false, false, "P <-> Q", true, "Basic bi-implication with true <-> true"},
        {true, false, false, false, "P <-> Q", false, "Basic bi-implication with true <-> false"},
        
        // Complex expressions
        {true, false, true, false, "(P & Q) | R", true, "Complex: (true & false) | true"},
        {true, true, false, true, "~(P & Q) | (R ^ S)", true, "Complex: NOT(true & true) | (false ^ true)"},
        {true, false, true, false, "(P -> Q) <-> (R | S)", false, "Complex: (true -> false) <-> (true | false)"},
        {true, true, false, true, "~P | (Q & R) -> (S ^ P)", true, "Complex expression with multiple operators"},
        {true, false, true, false, "(P -> Q) & (R -> S) | (~P & ~R)", false, "Complex expression with implications"},
        {true, false, true, false, "((P -> Q) & (R -> S)) <-> (~P | ~R)", true, "Complex nested implications"},
        {true, true, false, true, "(P & Q & ~R) -> ((S ^ P) | (Q -> R))", false, "Multiple AND chain with XOR and implication"},
        {true, false, true, true, "~(P & Q) <-> ((R -> S) & (P ^ Q))", true, "Negation of AND with bi-implication"},
        {false, true, false, true, "(((P -> Q) & (R -> S)) ^ (P | R)) -> (~Q <-> S)", false, "Nested implications with XOR"},
        {true, true, false, true, "(P -> Q) & (Q -> R) & (R -> S) & (S -> P)", false, "Chain of implications"},
        {false, true, true, false, "((P <-> Q) ^ (R <-> S)) -> (~P & Q | R & ~S)", true, "Compound bi-implications"},
        {true, false, true, true, "~(P -> Q) <-> (~R | S) & (P ^ (Q -> R))", false, "Negated implication"},
        {true, false, true, false, "(P & ~Q & R) -> ((S ^ P) | (~Q & ~R))", true, "Complex implication with multiple operations"},
        {true, false, false, true, "((P ^ Q) & (R -> S)) <-> (~P | (Q <-> R))", true, "Mixed operators with nested bi-implications"}
    };
    
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    printf("Running %d tests...\n\n", num_tests);
    
    for (int i = 0; i < num_tests; i++) {
        run_test_case(&test_cases[i]);
    }
}

int main(void) {
    run_tests();
    return 0;
}