#include <iostream>
#include <algorithm>
#include <stack>

#define LEFT '('
#define RIGHT ')'
#define AND '^'
#define OR 'V'
#define NEG '~'
#define IMPL '>'
#define UNARY 1
#define BINARY 2

using namespace std;

struct Node
{
    char data;
    Node* left;
    Node* right;
};

int operator_type(char token)
{
    switch(token)
    {
        case NEG:
            return UNARY;
        case LEFT:
        case RIGHT:
        case AND:
        case OR:
        case IMPL:
            return BINARY;
    }
    return 0;
}

string infix_to_postfix(string infix)
{
    string postfix;
    stack<char> st;
    for(auto token: infix)
    {
        if(operator_type(token))
            st.push(token);
        else
            postfix += token;
        if(operator_type(token) == UNARY)       // NEG
            continue;
        if(!st.empty() and st.top() == NEG)
        {
            postfix += st.top();
            st.pop();
        }
        if(!st.empty() and st.top() == RIGHT)
        {
            st.pop();                           // pop right bracket
            while(st.top() != LEFT)
            {
                postfix += st.top();
                st.pop();
            }
            st.pop();                           // pop left bracket
        }
    }
    while(!st.empty())
    {
        postfix += st.top();
        st.pop();
    }
    return postfix;
}

Node* postfix_to_parsetree(string postfix)
{
    stack<Node*> st;
    for(auto token: postfix)
    {
        Node* node = new Node();
        if(token == NEG)
        {
            node -> data = token;
            node -> right = st.top();
            node -> left = NULL;
            st.pop();
            st.push(node);
        }
        else if(operator_type(token))
        {
            node -> data = token;
            node -> right = st.top();
            st.pop();
            node -> left = st.top();
            st.pop();
            st.push(node);
        }
        else
        {
            node -> data = token;
            node -> right = NULL;
            node -> left = NULL;
            st.push(node);
        }
    }
    return st.top();
}

string parsetree_to_infix(Node* root)
{
    string infix = "";
    if(root == NULL)
        return infix;
    char token = root -> data;
    if(operator_type(token) == BINARY)
        infix += LEFT;
    infix += parsetree_to_infix(root -> left);
    infix += token;
    infix += parsetree_to_infix(root -> right);
    if(operator_type(token) == BINARY)
        infix += RIGHT;
    return infix;
}

int main()
{
    string infix, postfix;
    Node* tree;
    cin >> infix;
    postfix = infix_to_postfix(infix);
    cout << postfix << endl;
    tree = postfix_to_parsetree(postfix);
    infix = parsetree_to_infix(tree);
    cout << infix;
    return 0;
}