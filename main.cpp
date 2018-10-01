#include <iostream>
#include <stack>

#define LEFT_PAREN '('
#define RIGHT_PAREN ')'
#define AND '^'
#define OR 'V'
#define NEG '~'
#define IMPL '>'

using namespace std;

string infix_to_postfix(string infix)
{
    string postfix;
    stack<char> st;
    for(auto token: infix)
    {
        if(isalpha(token))
            postfix += token;
        else
            st.push(token);
        if(!st.empty() and st.top() == RIGHT_PAREN)
        {
            st.pop();
            while(st.top() != LEFT_PAREN)
            {
                postfix += st.top();
                st.pop();
            }
            st.pop();
        }
    }
    while(!st.empty())
    {
        postfix += st.top();
        st.pop();
    }
    return postfix;
}

int main()
{
    string s;
    cin >> s;
    cout << infix_to_postfix(s) << endl;
    return 0;
}