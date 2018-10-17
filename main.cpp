#include <iostream>
#include <stack>
#include <vector>

#define LEFT '('
#define RIGHT ')'
#define AND '^'
#define OR 'V'
#define NEG '~'
#define IMPL '>'
#define NONE 0
#define UNARY 1
#define BINARY 2
#define PREMISE "P"
#define AND_INTRO "^i"
#define AND_ELIM_1 "^e1"
#define AND_ELIM_2 "^e2"
#define OR_INTRO_1 "Vi1"
#define OR_INTRO_2 "Vi2"
#define IMPL_ELIM ">e"
#define MOD_TOL "MT"
#define OK 0
#define INVALID_RULE 1
#define INVALID_LINE 2

using namespace std;

struct Node
{
    char data;
    Node* left;
    Node* right;
};

typedef Node* parsetree;

struct Line
{
    string statement;
    string rule;
    int line1;
    int line2;
    Line()
    {
        statement = rule = "";
        line1 = line2 = 0;
    }
};

/** returns the type of an operator (unary, binary, or not an operator) */
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
    return NONE;
}

/** converts an infix expression to postfix */
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
        if(operator_type(token) == UNARY)
            continue;
        if(!st.empty() and operator_type(st.top()) == UNARY)
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

parsetree postfix_to_parsetree(string postfix)
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

string parsetree_to_infix(parsetree root)
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

parsetree infix_to_parsetree(string infix)
{
    return postfix_to_parsetree(infix_to_postfix(infix));
}

string parsetree_to_postfix(parsetree tree)
{
    return infix_to_postfix(parsetree_to_infix(tree));
}

int check_line(Line line, int line_no)
{
    if(line.rule == PREMISE)
    {
        if(line.line1 or line.line2)
            return INVALID_LINE;
    }
    else if(line.rule == AND_INTRO or line.rule == IMPL_ELIM
        or line.rule == MOD_TOL)
    {
        if(line.line1 < 1 or line.line1 >= line_no)
            return INVALID_LINE;
        if(line.line2 < 1 or line.line2 >= line_no)
            return INVALID_LINE;
    }
    else if(line.rule == AND_ELIM_1 or line.rule == AND_ELIM_2
        or line.rule == OR_INTRO_1 or line.rule == OR_INTRO_2)
    {
        if(line.line1 < 1 or line.line1 >= line_no)
            return INVALID_LINE;
        if(line.line2)
            return INVALID_LINE;
    }
    else
        return INVALID_RULE;
    return OK;
}

Line input_line(int line_number)
{
    Line line = Line();
    string in;
    cout << line_number << " ";
    getline(cin, in);
    string::iterator c = in.begin();
    for(; c != in.end() and *c != '/'; ++c)
        if(*c != ' ')
            line.statement += *c;
    if(c != in.end())
        c++;
    for(; c != in.end() and *c != '/'; ++c)
        if(*c != ' ')
            line.rule += *c;
    if(c != in.end())
        c++;
    for(; c != in.end() and *c != '/'; ++c)
    {
        if(*c != ' ')
            line.line1 = 10 * line.line1 + *c - '0';
        if(*c < '0' or *c > '9')
            line.line1 = -1;
    }
    if(c != in.end())
        c++;
    for(; c != in.end() and *c != '/'; ++c)
    {
        if(*c != ' ')
            line.line2 = 10 * line.line2 + *c - '0';
        if(*c < '0' or *c > '9')
            line.line2 = -1;
    }
    return line;
}

vector<Line> input_proof(int n)
{
    vector<Line> proof;
    for(int i = 1; i <= n; i++)
    {
        Line line;
        while(true)
        {
            line = input_line(i);
            int res = check_line(line, i);
            if(res == OK)
                break;
            else if(res == INVALID_RULE)
                cerr << "Invalid proof rule." << endl;
            else if(res == INVALID_LINE)
                cerr << "Invalid line number(s)." << endl;
        }
        proof.emplace_back(line);
    }
    return proof;
}

bool check_proof(vector<Line> proof)
{
    for(auto line: proof)
    {
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
        else if(line.rule == IMPL_ELIM)
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
        else if(line.rule == MOD_TOL)
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
    cout << (check_proof(proof) ? "V" : "Inv") << "alid proof";
    return 0;
}
