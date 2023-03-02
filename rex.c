

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

    /* pointer to an index in the expression-string to easily jump back after recursively 
     * parsing subexpressions (those surrounded by parentheses--groups and functions) */
    uint8_t* e_before_jump = NULL;

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
			
	    /* pattern of subexpression found in text, return to go back to the previous function stack frame */
	    case ')':
		++e;
	    return 0;
			
	    /* character classes; "a-z" "A-Z" "A-z" "0-9" */
	    case '[':
		
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
	++e;
	has_failed = (negated)? !has_failed: has_failed;
	has_failed = (has_failed && *e == '|')? false: true;
	
	 /* code to handle match multipliers--moving the expression pointer and changing the "has_failed" bool accordingly */
	int match_mul = parse_match_multiplier(e);
	switch (match_mul) {
	    case 0:
		if (negated) { match_mul = ANY; negated = false; }
		else negated = true;
				
		if (!has_failed) e = e_before_subexpr;
	    break;
	    case MM_NONEORONCE:
	        has_failed = false;
	    break;
	    case MM_ONCEORMORE:
		bool first = true;
		if (first && has_failed) return -1;
		else if (first) first = false;
		else if (has_failed) break;
	    break;
	    case MM_ANYNONGREEDY:
	    break;
	    case MM_ANY:
	    break;
	    default:
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
