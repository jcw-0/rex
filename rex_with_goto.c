

/* TODO: 
 *   match multipliers
 *   functions.. */ 
 
/* match multipliers */
#define MM_NONEORONCE   -1
#define MM_ONCEORMORE   -2
#define MM_ANYNONGREEDY -3
#define MM_ANY          -4


/*  the expression supports two modes: matching and substituting (whitespace is optional);
 *  matching:		'/pattern_to_match/' 
 *  substituting:	's/pattern_to_match/substituting_text/' 
 *
 *      progname '/pattern_to_match/ s/pattern_to_match_within_matched_pattern/new_text/' 
 *      progname 's/substitute_pattern/new_text/ /match'"*/

int regex(uint8_t* expression, uint8_t* input, uint8_t* output) {

    __label__ parse_expr_loop;
    __label__ parse_expression_loop;
    __label__ has_failed;
    __label__ log_err_and_die;
    __label__ sbrt;

    static bool first = true;
    uint8_t* e = expression, in = input, out = output;

    bool substitute 	= false;
    bool negated 	= false;
    uint8_t flags 	= 0;
    uint8_t modflags 	= 0;
    int match_counter   = 1;
    char delimiter 	= '\0';


    /* parse expression metadata, such as modifier flags and mode (match / substitute).
     * locked due to the usage of recursion */
    if (first) {

   	parse_flag_loop:
        
   	    switch (*e) {
	        
            case  ' '     :
	        case '\t'     : ++e; goto parse_flag_loop;
            case 's'      : substitute = true; ++e;
	        case delimiter: ++e;
	        default       : if (first) { delimiter = *e; ++e; }
	    }
    }

    /* program-loop; iterated for each character in the expression (exceptions exist where multiple characters
     * could be thought of as one, e.g function definitions "(?...)", escaped characters "\[alphanumerical]"*/
    parse_expr_loop:

    	if (!first) ++n;
	    first = false;

        switch (*(++e)) {
            
            case delimiter:     ++e; goto parse_flag_loop
            case  ' ':
	        case '\t':          goto parse_expr_loop; 
            case '!': negated = !negated; goto parse_expr_loop;
        		
            /* handle all expressions inside parentheses as subexpressions,
             * parse their content recursively */
            case '(':
        	    if (++e == '?')              goto sbrt;
        	    if (!0 == regex(e, in, out)) goto has_failed;
		        else 						 goto parse_expr_loop;
            /* pattern of subexpression found in text, return to go back to the previous function stack frame */
            case ')':
        	    ++e;
                return 0;

            /* character classes; "a-z" "A-Z" "A-z" "0-9" */
            case '[':
                goto has_failed;

            case '\\':
        	    ++e;
        	    if (*e == 'n' && *in == '\n') 			            goto parse_expression_loop;
        	    else if (*e == 't' && *in == '\t') 			        goto parse_expression_loop;
        	    else if (*e == 's' && (*in == ' ' || *in == '\t') 	goto parse_expression_loop;  
	   
	        /* expression was successfully matched, return to calling code */	
	        case '\0':
    		    return 0;

            default:
        	    if (!(*e == *in) && !negated) 			goto parse_expression_loop;
    		    if (*e == '|' || negated) 				goto parse_expression_loop;
        }



    /* executes when a pattern was not matched when it had to be */
    has_failed:	
	return -1;


    /* cleanup, log errors, die */
    log_err_and_die:
    return 1;


    /* messier subroutines */
    sbrt:
    ;
}
