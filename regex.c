#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAXEXPR     256
#define MAXSUBEXPR  256

/* flags */
#define GROUP       1<<0
#define CLASS       1<<1
/* function flags */
#define CAPTURE     1<<0
#define LOOKBEHIND  1<<1
#define LOOKAHEAD   1<<2

/* matchmultipliers */
#define MM_NONE          0 /* this is equivalent to negating */
#define MM_NONEORONCE   -1
#define MM_ONCEORMORE   -2
#define MM_ANYNONGREEDY -3
#define MM_ANY          -4

typedef struct subexpr_t {
    uint8_t* data;
    uint8_t flags;
    uint8_t fflags;
    int16_t n_matches;
    int n_childsubexpr;
    struct expr_t* childsubexpr[MAXEXPR];
} subexpr_t;



const subexpr_t e_def = {
        .data = NULL,
        .flags = 0,
        .fflags = 0,
        .n_matches = 0,
        .n_subexpr = 0,
        .subexpr = NULL
};

void error_and_die(char* errmsg, uint8_t* expr) {
    /* TODO: give valuable error message through referencing given regular expression */
    printf("%s\n", errmsg);
    exit 1;
}

static inline void parse_num_of_matches(subexpr_t* e, uint8_t* expr) {
    switch (*expression) {
        case '+':
            e->n_matches |= MM_ONCEORMORE;
        break;
        case '?':
            e->n_matches |= MM_NONEORONCE;
        break;
        case '*':
            if (*(expression + 1) == '?') { expression++; e->n_matches |= MM_ANYNONGREEDY;
            else e->n_matches |= MM_ANY;
        break;
        case '{':
            int n = 1;
            int mul = 1;
            *expression++;
            while (*expression != '}') { expression++; n++; }
            int n = 1;
            while (0 < (n--)) {
                e->n_matches += (*(expression - (i++)) ^ '0') * mul; mul *= 10;
            }
        break;
        default:
            e->n_matches = 1;
        return;
    }
    expression++;
}

int n_allocated = 0;
subexpr_t allocated[512];
static inline void create_subexpression(subexpr_t* e) {
    allocated[n_allocated] = malloc(sizeof (subexpr_t));
    (e->childsubexpr + e->n_childsubexpr) = allocated[n_allocated++];
    e->n_childsubexpr++;
}

static void tokenize(subexpr_t* e, uint8_t* expression) {

    *e = e_def;

    while (*expression) {
        switch (*expression) {
            case '(':
                if (e->flags & CLASS) error_and_die("error: cannot use (group) in [class]", expression);

                /* change the char to null-char for easy parsing when reading our subexpressions in future functions 
                 * this will be done each time a new subexpr starts and ends (except for when parsing alphanumericals as they
                 * are each parsed one by one (is that sane?)) */
                *expression = '\0'; expression++;

                create_subexpression(e);
                tokenize((e->childsubexpr + e->n_subchildexpr), expression);

                e->flags |= GROUP;

                /* ? = function mode */
                if (*expression == '?') {
                    expression++;
                    if (*expression == ':')      e->fflags |= CAPTURE;
                    else if (*expression == '<') e->fflags |= LOOKBEHIND;
                    else if (*expression == '>') e->fflags |= LOOKAHEAD;
                    else error_and_die("error: function mode used incorrectly", expression);
                }
            break;
            case ')':
                *expression = '\0'; expression++;
                parse_num_of_matches(e, expression);    
            return;
            case '[':
                *expression = '\0'; expression++;
                e->flags |= CLASS;
                create_subexpression(e);
                tokenize((e->childsubexpr + e->n_subchildexpr), expression);
            break;
            case ']':
                *expression = '\0'; expression++;
                parse_num_of_matches(e, expression);    
            return;
            case '\\':
                expression++;
                parse_num_of_matches(e, expression);    
            default:
                expression++;
                parse_num_of_matches(e, expression);    
            break;
        }
    }
    return;
}

int regex(uint8_t* expression, uint8_t* input, uint8_t* output) {

    subexpr_t* e = malloc(sizeof (subexpr_t*) * MAXSUBEXPR);
    tokenize(e, expression);
    // match(e, text);
    // if (substitute)
    //    substitute();
    free(e);

    return 0;
}

int main(void) {
    regex("\\((?>))", NULL, NULL);
    return 0;
}
