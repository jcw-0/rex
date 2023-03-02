

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
	uint8_t* e = expression, in = input, out = output;
	return match(e, in, out);
}

static int match(uint8_t* e, uint8_t* in, uint8_t* out) {
    
    bool has_failed 	= false;
    bool substitute 	= false;
    bool negated 	= false;
    uint8_t flags 	= 0;
    uint8_t modflags 	= 0;
	
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
		    if (*e != '|') 
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
		if (*e == 'n') { has_failed = ('\n' == *in)? false: true; ++e; }
		else if (*e == 't') { has_failed = ('\t' == *in)? false: true; ++e; }
		else if (*e == 's') { has_failed = (' '  == *in || '\t' == *in)? false: true; ++e; }
	    default:
		has_failed = (*e == *in)? false: true;
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
            return MM_ONCEORMORE;
        break;
        case '?':
            return MM_NONEORONCE;
        break;
        case '*':
            if (*(e + 1) == '?') return MM_ANYNONGREEDY;
            else return MM_ANY;
        break;
        case '{':
            int n = 1;
            int mul = 1;
            *e++;
            while (*e != '}') { e++; n++; }
            int i = 1;
			int accumulator = 0;
            while (0 < (n--)) { accumulator += (*(e - (i++)) ^ '0') * mul; mul *= 10; } 
			return accumulator;
        break;
        default:
            return = 1;
        return;
    }
    expression++;
}
