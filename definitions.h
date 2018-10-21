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

/** Represents a node in a rooted binary parse tree. */
struct Node
{
    char data;      ///< Contents of the node (i.e. operand or operator)
    Node* left;     ///< Reference to the left child of the node
    Node* right;    ///< Reference to the right child of the node
};

typedef Node* parsetree;
typedef std::string str;

/** Structure for a typical proof line. */
struct Line
{
    int number;         ///< Line number in the proof
    str statement;      ///< The statement or formula
    str rule;           ///< The proof rule used to arrive at that statement
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
Type pop(std::stack<Type>& st)
{
    if(st.empty())
        return 0;
    Type top = st.top();
    st.pop();
    return top;
}
