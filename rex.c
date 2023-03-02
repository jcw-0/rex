

/* TODO: 
 *   match multipliers
 *   functions.. */ 
 
 
/* match multipliers */
#define MM_NONEORONCE   -1
#define MM_ONCEORMORE   -2
#define MM_ANYNONGREEDY -3
#define MM_ANY          -4

static inline void parse_match_multiplier(uint8_t* e);

int regex(uint8_t* expression, uint8_t* input, uint8_t* output) {
    
    /* make a new pointer to each input to allow pointer arithmetic without messing with passed in pointers */
    uint8_t* e = expression, in = input, out = output;

    return match(e, in, out);
}

static int match(uint8_t* e, uint8_t* in, uint8_t* out) {
    
    bool has_failed 	= false;
    bool substitute 	= false;
    bool negated 	= false;
    uint8_t flags 	= 0;
    uint8_t modflags 	= 0;

    /* pointer to a index in the expression-string for easy jumping back after recursively 
     * parsing subexpressions (those surrounded by parentheses--groups and functions) */
    uint8_t* e_before_jump = NULL;
	
    start_parse_loop:
    while (!has_failed) {
        switch (*e) {
	    case ' ':
		e++;
	    break;
	    case '!':
		negated = !negated;
		e++;
	    break;
			
	    /* handle all expressions inside parentheses as subexpressions,
	     * parse their content recursively */
	    case '(':
		if (++e == '?') 
		e_before_jump = e;
		if (!0 == match(e, input, output)) 
		    has_failed = true;
	    break;
	    case '?':
		if (is_function) 
	    case ')':
		++e;
		return 0;
	    case '':
	    break;
	    case '\\':
		++e;
		if 	(*e == 'n') { has_failed = ('\n' == *in)? false: true; ++e; }
		else if (*e == 't') { has_failed = ('\t' == *in)? false: true; ++e; }
		else if (*e == 's') { has_failed = (' '  == *in || '\t' == *in)? false: true; ++e; }
	    default:
		has_failed = (*e == *in)? false: true;
		has_failed = (negated)? !has_failed: has_failed;
	    break;
	}
		
	has_failed = (negated)? !has_failed: has_failed;
	has_failed = (has_failed && *(++e) == '|')? false: true;
		
	int match_mul = parse_match_multiplier(e);
	switch (match_mul) {
	    case 0:
		if (negated) { match_mul = ANY; negated = false; }
		else negated = true;
				
		if (!has_failed) e = e_before_subexpr;
	    break;
	    case MM_NONEORONCE:
		goto start_parse_loop;
	    case MM_ONCEORMORE:
	    break;
	}
    }
    return 0;
}

static inline int parse_match_multiplier(uint8_t* e) {
    switch (*e) {
        case '+':
	    ++e;
            return MM_ONCEORMORE;
        break;
        case '?':
	    ++e;
            return MM_NONEORONCE;
        break;
        case '*':
            if (*(++e) == '?') { ++e; return MM_ANYNONGREEDY; }
            else { ++e; return MM_ANY; }
        break;
		    
	/* casts the integer inbetween the squiggly brackets "{n}" to an integer and 
	 * uses it as a match multiplier */
        case '{':
            int n = 1, mul = 1, i = 1, accumulator = 0;
            *e++;
            while (*e != '}') { e++; n++; }
            while (0 < (n--)) { accumulator += (*(e - (i++)) ^ '0') * mul; mul *= 10; } 
	    ++e; 
	    return accumulator;
        break;
	/* if no match multiplier is declared, use 1 */
        default:
            return = 1;
        return;
    }
}
