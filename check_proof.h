/** Checks if a proof is valid. Returns true if it is, false otherwise. */
bool check_proof(std::vector<Line> proof)
{
    // Iterate through the proof:
    for(auto line: proof)
    {
        // Check what rule was used to arrive at the current line, then check
        // if the rule was used correctly. If it wasn't, return false.
        // If using a parsetree to check, erase it to avoid memory leaks.

        bool result = true;

        if(line.rule == PREMISE);
        else if(line.rule == AND_INTRO)
        {
            str cur_postfix = infix_to_postfix(line.statement);
            str left_infix = proof[line.line1 - 1].statement;
            str right_infix = proof[line.line2 - 1].statement;
            str left_postfix = infix_to_postfix(left_infix);
            str right_postfix = infix_to_postfix(right_infix);
            str full_postfix = left_postfix + right_postfix + AND;
            if(cur_postfix != full_postfix)
                return false;
        }
        else if(line.rule == AND_ELIM_1)
        {
            str cur_infix = line.statement;
            str parent_infix = proof[line.line1 - 1].statement;
            parsetree parent_tree = infix_to_parsetree(parent_infix);
            if(parent_tree -> data != AND)
                result = false;
            str left_infix = parsetree_to_infix(parent_tree -> left);
            if(left_infix != cur_infix)
                result = false;
            erase(parent_tree);
            if(result == false)
                return result;
        }
        else if(line.rule == AND_ELIM_2)
        {
            str cur_infix = line.statement;
            str parent_infix = proof[line.line1 - 1].statement;
            parsetree parent_tree = infix_to_parsetree(parent_infix);
            if(parent_tree -> data != AND)
                result = false;
            str right_infix = parsetree_to_infix(parent_tree -> right);
            if(right_infix != cur_infix)
                result = false;
            erase(parent_tree);
            if(result == false)
                return result;
        }
        else if(line.rule == OR_INTRO_1)
        {
            str cur_infix = line.statement;
            parsetree cur_tree = infix_to_parsetree(cur_infix);
            if(cur_tree -> data != OR)
                result = false;
            str left_infix = parsetree_to_infix(cur_tree -> left);
            str ref_infix = proof[line.line1 - 1].statement;
            if(left_infix != ref_infix)
                result = false;
            erase(cur_tree);
            if(result == false)
                return result;
        }
        else if(line.rule == OR_INTRO_2)
        {
            str cur_infix = line.statement;
            parsetree cur_tree = infix_to_parsetree(cur_infix);
            if(cur_tree -> data != OR)
                result = false;
            str right_infix = parsetree_to_infix(cur_tree -> right);
            str ref_infix = proof[line.line1 - 1].statement;
            if(right_infix != ref_infix)
                result = false;
            erase(cur_tree);
            if(result == false)
                return result;
        }
        else if(line.rule == IMPL_ELIM or line.rule == MODUS_PON)
        {
            str cur_infix = line.statement;
            str ref_infix = proof[line.line2 - 1].statement;
            str orig_infix = proof[line.line1 - 1].statement;
            str ref_postfix = infix_to_postfix(ref_infix);
            str cur_postfix = infix_to_postfix(cur_infix);
            str orig_postfix = infix_to_postfix(orig_infix);
            if(orig_postfix != ref_postfix + cur_postfix + IMPL)
                return false;
        }
        else if(line.rule == D_NEG_ELIM)
        {
            str cur_infix = line.statement;
            str ref_infix = proof[line.line1 - 1].statement;
            str cur_postfix = infix_to_postfix(cur_infix);
            str ref_postfix = infix_to_postfix(ref_infix);
            if(ref_postfix != cur_postfix + NEG + NEG)
                return false;
        }
        else if(line.rule == D_NEG_INTRO)
        {
            str cur_infix = line.statement;
            str ref_infix = proof[line.line1 - 1].statement;
            str cur_postfix = infix_to_postfix(cur_infix);
            str ref_postfix = infix_to_postfix(ref_infix);
            if(cur_postfix != ref_postfix + NEG + NEG)
                return false;
        }
        else if(line.rule == MODUS_TOL)
        {
            str cur_infix = line.statement;
            str ref_infix = proof[line.line2 - 1].statement;
            str orig_infix = proof[line.line1 - 1].statement;
            parsetree cur_tree = infix_to_parsetree(cur_infix);
            parsetree ref_tree = infix_to_parsetree(ref_infix);
            parsetree cur_root = cur_tree, ref_root = ref_tree;
            if(cur_tree -> data != NEG or ref_tree -> data != NEG)
                result = false;
            cur_tree = cur_tree -> right;
            ref_tree = ref_tree -> right;
            str cur_postfix = parsetree_to_postfix(cur_tree);
            str ref_postfix = parsetree_to_postfix(ref_tree);
            str orig_postfix = infix_to_postfix(orig_infix);
            if(orig_postfix != cur_postfix + ref_postfix + IMPL)
                result = false;
            erase(cur_root);
            erase(ref_root);
            if(result == false)
                return result;
        }
    }
    return true;
}
