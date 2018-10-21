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
str infix_to_postfix(str infix)
{
    str postfix;         // String to store the postfix expression.
    std::stack<char> st; // Stack to store and pop operators based on priority.

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
parsetree postfix_to_parsetree(str postfix)
{
    std::stack<Node*> st;       // Stack to store roots of parse trees.

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
str parsetree_to_infix(parsetree root)
{
    str infix = "";  // String to store the infix expression.

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
parsetree infix_to_parsetree(str infix)
{
    return postfix_to_parsetree(infix_to_postfix(infix));
}

/** Returns the postfix expression equivalent to the given parse tree. */
str parsetree_to_postfix(parsetree tree)
{
    return infix_to_postfix(parsetree_to_infix(tree));
}

/** Returns the infix form of an postfix expression. */
str postfix_to_infix(str postfix)
{
    return parsetree_to_infix(postfix_to_parsetree(postfix));
}
