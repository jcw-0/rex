#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#define MAXEXPR     256
#define GROUP       1<<0
#define CAPTURE     1<<1
#define LOOKBEHIND  1<<2
#define LOOKAHEAD   1<<3

typedef struct expr_t {
    uint8_t* data;
    uint8_t flags;
    int16_t n_matches;
    int n_subexpr;
    struct expr_t* subexpr[MAXEXPR];
} expr_t;


const expr_t e_def = {
        .data = NULL,
        .flags = 0,
        .n_matches = 1,
        .n_subexpr = 0,
        .subexpr = NULL
};

static int n_expr = 0;
static expr_t* e_ptr;
void tokenize(uint8_t* expression) {
    
    bool first_match_of_subexpr = true;    
    *(e_ptr + n_expr) = e_def;

    while (*expression) {
        switch (*expression) {
            case '(':
                if (!first_match_of_subexpr) {
                    n_expr++;
                    (e_ptr + n_expr)->subexpr[(e_ptr + n_expr)->n_subexpr++] = (e_ptr + n_expr + 1);
                    tokenize(expression);
                } first_match_of_subexpr = false;
                
                (e_ptr + n_expr)->flags |= GROUP;
                expression++;
                /* ? = function-mode */
                if (*expression == '?') {
                    expression += 1;
                    if (*expression == ':')
                        (e_ptr + n_expr)->flags |= CAPTURE;
                    else if (*expression == '<')
                        (e_ptr + n_expr)->flags |= LOOKBEHIND;
                    else if (*expression == '>')
                        (e_ptr + n_expr)->flags |= LOOKAHEAD;
                }
            break;
            case ')':
                /* check for num of matches */
            return;
            case '\\':
                expression++;
                expression += (*expression == '(')? 1: 0;
            break;
            default:
                expression++;
            break;
        }
    }
}

int regex(uint8_t* expression, uint8_t* text, uint8_t* destination) {
    e_ptr = malloc(sizeof(expr_t*) * 256);
    *e_ptr = e_def;

    tokenize(expression);
    // match(e, text);
    // if(substitute)
    //    substitute();

    /* for testing purposes */
    printf("flags:\n");
    for (int i = 0; i <= n_expr; i++)
        printf("\t%d\n", (e_ptr + i)->flags);
    
    return 0;
}

int main(void) {
    regex("\\((?>))", NULL, NULL);
    return 0;
}