/** Checks if a given infix expression is valid and strictly paranthesized. */
bool check_infix(str infix)
{
    // If number of left brackets is not the same as right, return false.
    int bracket_count = 0;
    for(auto token: infix)
    {
        if(token == LEFT)
            bracket_count++;
        if(token == RIGHT)
            bracket_count--;
    }
    if(bracket_count)
        return false;

    // If the expression matches itself after converting to postfix and back,
    // it is valid.
    return infix == postfix_to_infix(infix_to_postfix(infix));
}

/** Checks if a line is valid. Returns the error if invalid, OK otherwise. */
int check_line(Line line)
{
    // Check if the statement (infix) is valid.
    if(check_infix(line.statement) == false)
        return INVALID_STATEMENT;

    // Check if the rule is valid. Within each rule, check if the line numbers
    // are valid.

    // Rules having 0 line references.
    if(line.rule == PREMISE)
    {
        if(line.line1 or line.line2)
            return INVALID_LINE;
    }

    // Rules having 1 line reference.
    else if(line.rule == AND_ELIM_1
         or line.rule == AND_ELIM_2
         or line.rule == OR_INTRO_1
         or line.rule == OR_INTRO_2
         or line.rule == D_NEG_ELIM
         or line.rule == D_NEG_INTRO)
    {
        if(line.line1 < 1 or line.line1 >= line.number)
            return INVALID_LINE;
        if(line.line2)
            return INVALID_LINE;
    }

    // Rules having 2 line references.
    else if(line.rule == AND_INTRO
         or line.rule == IMPL_ELIM
         or line.rule == MODUS_TOL)
    {
        if(line.line1 < 1 or line.line1 >= line.number)
            return INVALID_LINE;
        if(line.line2 < 1 or line.line2 >= line.number)
            return INVALID_LINE;
    }

    // If the rule matched none of the above, it is invalid.
    else
        return INVALID_RULE;

    return OK;
}

/** Takes a typical proof rule line as input and returns it as an object of 
    struct Line. Format of the line to be input:
    <statement> / <proof-rule> / <ref1> / <ref2> */
Line input_line(int line_number)
{
    Line line = Line();                 // Object to store the line.
    line.number = line_number;          // Store the proof line number.
    std::cout << line.number << " ";    // Prompt for input.
    str in;
    getline(std::cin, in);

    // Iterate through the string:
    str::iterator c = in.begin();

    // Add characters to line.statement until '/' is encountered.
    for(; c != in.end() and *c != '/'; ++c)
        if(*c != ' ')
            line.statement += *c;
    if(c != in.end())
        c++;                            // Move past the '/'

    // Add characters to line.rule until '/' is encountered.
    for(; c != in.end() and *c != '/'; ++c)
        if(*c != ' ')
            line.rule += *c;
    if(c != in.end())
        c++;                            // Move past the '/'

    // To store integers, the algorithm used is such that it forces a negative
    // value to be stored if any non-numeric character is entered. Such a value
    // will be detected as invalid within check_line.

    // Take input for first line reference (line.line1)
    for(; c != in.end() and *c != '/'; ++c)
    {
        if(*c != ' ')
            line.line1 = 10 * line.line1 + *c - '0';
        if(*c < '0' or *c > '9')
            line.line1 = -1;
    }
    if(c != in.end())
        c++;                            // Move past the '/'

    // Take input for second line reference (line.line2)
    for(; c != in.end() and *c != '/'; ++c)
    {
        if(*c != ' ')
            line.line2 = 10 * line.line2 + *c - '0';
        if(*c < '0' or *c > '9')
            line.line2 = -1;
    }

    return line;
}

/** Takes a typical proof as input, stores individial lines in a vector and
    returns it. */
std::vector<Line> input_proof(int n)
{
    std::vector<Line> proof;            // A vector to store the proof.

    // Loop to take n lines of input.
    for(int i = 1; i <= n; i++)
    {
        Line line;                      // Line object to store a line.

        // Keep looping until a valid line is input. Output corresponding
        // error messages if any.
        while(true)
        {
            line = input_line(i);
            int res = check_line(line);
            if(res == OK)
                break;
            else if(res == INVALID_STATEMENT)
                std::cerr << "Invalid statement." << std::endl;
            else if(res == INVALID_RULE)
                std::cerr << "Invalid proof rule." << std::endl;
            else if(res == INVALID_LINE)
                std::cerr << "Invalid line number(s)." << std::endl;
        }

        // Add the line to proof.
        proof.emplace_back(line);
    }

    return proof;
}
