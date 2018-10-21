#include <iostream>
#include <stack>
#include <vector>

/* Operators */
#define LEFT '('                ///< Left paranthesis
#define RIGHT ')'               ///< Right paranthesis
#define AND '^'                 ///< Logical AND
#define OR 'V'                  ///< Logical OR
#define NEG '~'                 ///< Logical NEGATION or Logical NOT
#define IMPL '>'                ///< Logical IMPLICATION

/* Types of operators, return values for operator_type */
#define BRACE -1                ///< Operator is a paranthesis
#define NONE 0                  ///< Not an operator, i.e. operand
#define UNARY 1                 ///< Operator is unary
#define BINARY 2                ///< Operator is bianry

/* Proof rules */
#define PREMISE "P"             ///< Premise
#define AND_INTRO "^i"          ///< AND Introduction
#define AND_ELIM_1 "^e1"        ///< AND Elimination 1
#define AND_ELIM_2 "^e2"        ///< AND Elimination 2
#define OR_INTRO_1 "Vi1"        ///< OR Introduction 1
#define OR_INTRO_2 "Vi2"        ///< OR Introduction 2
#define IMPL_ELIM ">e"          ///< IMPLICATION Elimination
#define MODUS_PON "MP"          ///< Modus Ponens
#define D_NEG_INTRO "~~i"       ///< Double NEGATION Introduction 
#define D_NEG_ELIM "~~e"        ///< Double NEGATION Elimination
#define MODUS_TOL "MT"          ///< Modus Tollens

/* Possible errors in input, return values for check_line */
#define OK 0                    ///< Input is OK
#define INVALID_STATEMENT 1     ///< The statement is invalid
#define INVALID_RULE 2          ///< The rule is invalid
#define INVALID_LINE 3          ///< The line(s) referenced is/are invalid

using namespace std;

/** Represents a node in a rooted binary parse tree. */
struct Node
{
    char data;      ///< Contents of the node (i.e. operand or operator)
    Node* left;     ///< Reference to the left child of the node
    Node* right;    ///< Reference to the right child of the node
};

typedef Node* parsetree;

/** Structure for a typical proof line. */
struct Line
{
    int number;         ///< Line number in the proof
    string statement;   ///< The statement or formula
    string rule;        ///< The proof rule used to arrive at that statement
    int line1;          ///< The first line referred to by the proof rule
    int line2;          ///< The second line referred to by the proof rule

    /** Constructor initializes all variables to empty string or 0. */
    Line()
    {
        statement = rule = "";
        number = line1 = line2 = 0;
    }
};

/** Custom function for popping a stack while returning the top simultaneously.
    Works for stacks of primitive data types only. */
template<typename Type>
Type pop(stack<Type>& st)
{
    if(st.empty())
        return 0;
    Type top = st.top();
    st.pop();
    return top;
}

/** Returns the type of an operator (unary, binary, or none). */
int operator_type(char token)
{
    // Compare token with known operators.
    switch(token)
    {
        case LEFT:
        case RIGHT:
            return BRACE;
        case NEG:
            return UNARY;
        case AND:
        case OR:
        case IMPL:
            return BINARY;
    }

    // If no match was found, return NONE.
    return NONE;
}

/** Returns the postfix form of an infix expression. */
string infix_to_postfix(string infix)
{
    string postfix;     // String to store the postfix expression.
    stack<char> st;     // Stack to store and pop operators based on priority.

    // Iterate through the infix expression:
    for(auto token: infix)
    {
        // If the token is an operator, push it onto the stack;
        // if it's an operand, append it to postfix.
        if(operator_type(token) != NONE)
            st.push(token);
        else
            postfix += token;

        // If right bracket is found, pop it. Pop and append all the following
        // operators until but excluding left bracket.
        if(!st.empty() and st.top() == RIGHT)
        {
            pop(st);                            // pop right bracket
            while(st.top() != LEFT)
                postfix += pop(st);
            pop(st);                            // pop left bracket
        }
        
        // Since unary operator has high priority, pop it as soon as it follows
        // an operand/formula, but it must follow something.
        if(operator_type(token) == UNARY)
            continue;
        while(!st.empty() and operator_type(st.top()) == UNARY)
            postfix += pop(st);
    }

    // Pop and append all the remaining operators from the stack to postfix.
    while(!st.empty())
        postfix += pop(st);

    return postfix;
}

/** Builds a rooted binary parse tree from an expression in postfix form
    and returns the root of the parse tree. */
parsetree postfix_to_parsetree(string postfix)
{
    stack<Node*> st;    // Stack to store roots of parse trees.

    // Iterate through the postfix expression:
    for(auto token: postfix)
    {
        // Create a new node to store the token in.
        Node* node = new Node();
        node -> data = token;

        // If the token is not an operator, it has no children.
        if(operator_type(token) == NONE)
        {
            node -> right = NULL;
            node -> left = NULL;
        }

        // If the operator is unary, it has one child (to the right).
        if(operator_type(token) == UNARY)
        {
            node -> right = pop(st);
            node -> left = NULL;
        }

        // If the operator is binary, it has two children.
        if(operator_type(token) == BINARY)
        {
            node -> right = pop(st);
            node -> left = pop(st);
        }

        // Push the node onto the stack after assigning it children.
        st.push(node);
    }

    // If the postfix expression was valid, there must be exactly one node
    // in the stack after the iteration, i.e. the root of the parse tree.
    // If the postfix expression was blank, the stack will be empty.
    if(st.empty())
        return NULL;
    return st.top();
}

/** Returns the infix expression equivalent to the given parse tree. */
string parsetree_to_infix(parsetree root)
{
    string infix = "";  // String to store the infix expression.

    // If the parse tree is empty, return an empty string.
    if(root == NULL)
        return infix;

    char token = root -> data;

    // Append the infix of left child, the token and the infix of right child
    // to infix. If operator is binary, enclose infix in parantheses.
    if(operator_type(token) == BINARY)
        infix += LEFT;                              // append left bracket
    infix += parsetree_to_infix(root -> left);
    infix += token;
    infix += parsetree_to_infix(root -> right);
    if(operator_type(token) == BINARY)
        infix += RIGHT;                             // append right bracket

    return infix;
}

/** Builds a rooted binary parse tree from an expression in infix form
    and returns the root of the parse tree. */
parsetree infix_to_parsetree(string infix)
{
    return postfix_to_parsetree(infix_to_postfix(infix));
}

/** Returns the postfix expression equivalent to the given parse tree. */
string parsetree_to_postfix(parsetree tree)
{
    return infix_to_postfix(parsetree_to_infix(tree));
}

/** Returns the infix form of an postfix expression. */
string postfix_to_infix(string postfix)
{
    return parsetree_to_infix(postfix_to_parsetree(postfix));
}

/** Checks if a given infix expression is valid and strictly paranthesized. */
bool check_infix(string infix)
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
    cout << line.number << " ";         // Prompt for input.
    string in;
    getline(cin, in);

    // Iterate through the string:
    string::iterator c = in.begin();

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
vector<Line> input_proof(int n)
{
    vector<Line> proof;                 // A vector to store the proof.

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
                cerr << "Invalid statement." << endl;
            else if(res == INVALID_RULE)
                cerr << "Invalid proof rule." << endl;
            else if(res == INVALID_LINE)
                cerr << "Invalid line number(s)." << endl;
        }

        // Add the line to proof.
        proof.emplace_back(line);
    }

    return proof;
}

/** Checks if a proof is valid. Returns true if it is, false otherwise. */
bool check_proof(vector<Line> proof)
{
    // Iterate through the proof:
    for(auto line: proof)
    {
        // Check what rule was used to arrive at the current line, then check
        // if the rule was used correctly. If it wasn't, return false.
        if(line.rule == PREMISE);
        else if(line.rule == AND_INTRO)
        {
            string cur_postfix = infix_to_postfix(line.statement);
            string left_infix = proof[line.line1 - 1].statement;
            string right_infix = proof[line.line2 - 1].statement;
            string left_postfix = infix_to_postfix(left_infix);
            string right_postfix = infix_to_postfix(right_infix);
            string full_postfix = left_postfix + right_postfix + AND;
            if(cur_postfix != full_postfix)
                return false;
        }
        else if(line.rule == AND_ELIM_1)
        {
            string cur_infix = line.statement;
            string parent_infix = proof[line.line1 - 1].statement;
            parsetree parent_tree = infix_to_parsetree(parent_infix);
            if(parent_tree -> data != AND)
                return false;
            string left_infix = parsetree_to_infix(parent_tree -> left);
            if(left_infix != cur_infix)
                return false;
        }
        else if(line.rule == AND_ELIM_2)
        {
            string cur_infix = line.statement;
            string parent_infix = proof[line.line1 - 1].statement;
            parsetree parent_tree = infix_to_parsetree(parent_infix);
            if(parent_tree -> data != AND)
                return false;
            string right_infix = parsetree_to_infix(parent_tree -> right);
            if(right_infix != cur_infix)
                return false;
        }
        else if(line.rule == OR_INTRO_1)
        {
            string cur_infix = line.statement;
            parsetree cur_tree = infix_to_parsetree(cur_infix);
            if(cur_tree -> data != OR)
                return false;
            string left_infix = parsetree_to_infix(cur_tree -> left);
            string ref_infix = proof[line.line1 - 1].statement;
            if(left_infix != ref_infix)
                return false;
        }
        else if(line.rule == OR_INTRO_2)
        {
            string cur_infix = line.statement;
            parsetree cur_tree = infix_to_parsetree(cur_infix);
            if(cur_tree -> data != OR)
                return false;
            string right_infix = parsetree_to_infix(cur_tree -> right);
            string ref_infix = proof[line.line1 - 1].statement;
            if(right_infix != ref_infix)
                return false;
        }
        else if(line.rule == IMPL_ELIM or line.rule == MODUS_PON)
        {
            string cur_infix = line.statement;
            string ref_infix = proof[line.line2 - 1].statement;
            string orig_infix = proof[line.line1 - 1].statement;
            string ref_postfix = infix_to_postfix(ref_infix);
            string cur_postfix = infix_to_postfix(cur_infix);
            string orig_postfix = infix_to_postfix(orig_infix);
            if(orig_postfix != ref_postfix + cur_postfix + IMPL)
                return false;
        }
        else if(line.rule == D_NEG_ELIM)
        {
            string cur_infix = line.statement;
            string ref_infix = proof[line.line1 - 1].statement;
            string cur_postfix = infix_to_postfix(cur_infix);
            string ref_postfix = infix_to_postfix(ref_infix);
            if(ref_postfix != cur_postfix + NEG + NEG)
                return false;
        }
        else if(line.rule == D_NEG_INTRO)
        {
            string cur_infix = line.statement;
            string ref_infix = proof[line.line1 - 1].statement;
            string cur_postfix = infix_to_postfix(cur_infix);
            string ref_postfix = infix_to_postfix(ref_infix);
            if(cur_postfix != ref_postfix + NEG + NEG)
                return false;
        }
        else if(line.rule == MODUS_TOL)
        {
            string cur_infix = line.statement;
            string ref_infix = proof[line.line2 - 1].statement;
            string orig_infix = proof[line.line1 - 1].statement;
            parsetree cur_tree = infix_to_parsetree(cur_infix);
            parsetree ref_tree = infix_to_parsetree(ref_infix);
            if(cur_tree -> data != NEG or ref_tree -> data != NEG)
                return false;
            cur_tree = cur_tree -> right;
            ref_tree = ref_tree -> right;
            string cur_postfix = parsetree_to_postfix(cur_tree);
            string ref_postfix = parsetree_to_postfix(ref_tree);
            string orig_postfix = infix_to_postfix(orig_infix);
            if(orig_postfix != cur_postfix + ref_postfix + IMPL)
                return false;
        }
    }
    return true;
}

int main()
{
    int n;
    cin >> n;
    cin.ignore();
    vector<Line> proof = input_proof(n);
    if(check_proof(proof))
        cout << "Valid proof.";
    else
        cout << "Invalid proof.";
    return 0;
}
