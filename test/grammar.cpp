#include "grammar.h"
#include <algorithm>

const string EPSILON = "EPSILON";

// Build the LL(1) grammar as a vector of productions.
vector<Production> buildGrammar() {
    vector<Production> grammar;
    // 1. <sql> -> <statement> ";" <sql_tail>
    grammar.push_back({"<sql>", {"<statement>", ";", "<sql_tail>"}});
    // 2. <sql_tail> -> <statement> ";" <sql_tail>
    grammar.push_back({"<sql_tail>", {"<statement>", ";", "<sql_tail>"}});
    // 3. <sql_tail> -> EPSILON
    grammar.push_back({"<sql_tail>", {EPSILON}});
    
    // 4. <statement> -> <create_table_stmt>
    grammar.push_back({"<statement>", {"<create_table_stmt>"}});
    // 5. <statement> -> <insert_stmt>
    grammar.push_back({"<statement>", {"<insert_stmt>"}});
    // 6. <statement> -> <select_stmt>
    grammar.push_back({"<statement>", {"<select_stmt>"}});
    
    // 7. <create_table_stmt> -> "CREATE" "TABLE" <identifier> "(" <column_def_list> "," "PRIMARY" "KEY" "(" <identifier> ")" ")"
    grammar.push_back({"<create_table_stmt>", {"CREATE", "TABLE", "<identifier>", "(", "<column_def_list>", ",", "PRIMARY", "KEY", "(", "<identifier>", ")", ")"}});
    
    // 8. <column_def_list> -> <column_def> <column_def_list_tail>
    grammar.push_back({"<column_def_list>", {"<column_def>", "<column_def_list_tail>"}});
    // 9. <column_def_list_tail> -> "," <column_def> <column_def_list_tail>
    grammar.push_back({"<column_def_list_tail>", {",", "<column_def>", "<column_def_list_tail>"}});
    // 10. <column_def_list_tail> -> EPSILON
    grammar.push_back({"<column_def_list_tail>", {EPSILON}});
    
    // 11. <column_def> -> <identifier> <datatype>
    grammar.push_back({"<column_def>", {"<identifier>", "<datatype>"}});
    
    // 12. <insert_stmt> -> "INSERT" "INTO" <identifier> <opt_identifier_list> "VALUES" "(" <value_list> ")"
    grammar.push_back({"<insert_stmt>", {"INSERT", "INTO", "<identifier>", "<opt_identifier_list>", "VALUES", "(", "<value_list>", ")"}});
    // 13. <opt_identifier_list> -> "(" <identifier_list> ")"
    grammar.push_back({"<opt_identifier_list>", {"(", "<identifier_list>", ")"}});
    // 14. <opt_identifier_list> -> EPSILON
    grammar.push_back({"<opt_identifier_list>", {EPSILON}});
    
    // 15. <identifier_list> -> <identifier> <identifier_list_tail>
    grammar.push_back({"<identifier_list>", {"<identifier>", "<identifier_list_tail>"}});
    // 16. <identifier_list_tail> -> "," <identifier> <identifier_list_tail>
    grammar.push_back({"<identifier_list_tail>", {",", "<identifier>", "<identifier_list_tail>"}});
    // 17. <identifier_list_tail> -> EPSILON
    grammar.push_back({"<identifier_list_tail>", {EPSILON}});
    
    // 18. <value_list> -> <value> <value_list_tail>
    grammar.push_back({"<value_list>", {"<value>", "<value_list_tail>"}});
    // 19. <value_list_tail> -> "," <value> <value_list_tail>
    grammar.push_back({"<value_list_tail>", {",", "<value>", "<value_list_tail>"}});
    // 20. <value_list_tail> -> EPSILON
    grammar.push_back({"<value_list_tail>", {EPSILON}});
    
    // 21. <select_stmt> -> "SELECT" <select_list> "FROM" <identifier> <opt_where_clause> ";"
    grammar.push_back({"<select_stmt>", {"SELECT", "<select_list>", "FROM", "<identifier>", "<opt_where_clause>", ";"}});
    
    // 22. <select_list> -> "*"
    grammar.push_back({"<select_list>", {"*"}});
    // 23. <select_list> -> <identifier_list>
    grammar.push_back({"<select_list>", {"<identifier_list>"}});
    
    // 24. <opt_where_clause> -> <where_clause>
    grammar.push_back({"<opt_where_clause>", {"<where_clause>"}});
    // 25. <opt_where_clause> -> EPSILON
    grammar.push_back({"<opt_where_clause>", {EPSILON}});
    
    // 26. <where_clause> -> "WHERE" <condition>
    grammar.push_back({"<where_clause>", {"WHERE", "<condition>"}});
    
    // 27. <condition> -> <identifier> <comp_operator> <value>
    grammar.push_back({"<condition>", {"<identifier>", "<comp_operator>", "<value>"}});
    // 28. <condition> -> <identifier> "BETWEEN" <value> "AND" <value>
    grammar.push_back({"<condition>", {"<identifier>", "BETWEEN", "<value>", "AND", "<value>"}});
    // 29. <condition> -> <identifier> "LIKE" <value>
    grammar.push_back({"<condition>", {"<identifier>", "LIKE", "<value>"}});
    // 30. <condition> -> <identifier> "IN" "(" <value_list> ")"
    grammar.push_back({"<condition>", {"<identifier>", "IN", "(", "<value_list>", ")"}});
    
    // 31. <comp_operator> -> "="
    grammar.push_back({"<comp_operator>", {"="}});
    // 32. <comp_operator> -> ">"
    grammar.push_back({"<comp_operator>", {">"}});
    // 33. <comp_operator> -> "<"
    grammar.push_back({"<comp_operator>", {"<"}});
    
    // 34. <identifier> -> id      (placeholder for an identifier token)
    grammar.push_back({"<identifier>", {"id"}});
    // 35. <datatype> -> <identifier>
    grammar.push_back({"<datatype>", {"<identifier>"}});
    
    // 36. <value> -> <number>
    grammar.push_back({"<value>", {"<number>"}});
    // 37. <value> -> <string>
    grammar.push_back({"<value>", {"<string>"}});
    // 38. <value> -> <identifier>
    grammar.push_back({"<value>", {"<identifier>"}});
    
    // 39. <number> -> num      (placeholder for a numeric literal)
    grammar.push_back({"<number>", {"num"}});
    // 40. <string> -> str      (placeholder for a string literal)
    grammar.push_back({"<string>", {"str"}});
    
    return grammar;
}

// Compute the FIRST sets for all grammar symbols.
map<string, set<string>> computeFirstSets(const vector<Production>& grammar) {
    map<string, set<string>> first;
    
    // Initialize FIRST sets for terminals.
    auto addTerminalFirst = [&](const string &sym) {
        if (!isNonTerminal(sym) && sym != EPSILON)
            first[sym].insert(sym);
    };
    for (const auto &prod : grammar) {
        if (first.find(prod.lhs) == first.end())
            first[prod.lhs] = set<string>();
        for (const auto &sym : prod.rhs) {
            addTerminalFirst(sym);
        }
    }
    first[EPSILON].insert(EPSILON);
    
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto &prod : grammar) {
            const string &A = prod.lhs;
            set<string> firstAlpha;
            bool allEpsilon = true;
            for (const auto &sym : prod.rhs) {
                for (const auto &item : first[sym]) {
                    if (item != EPSILON)
                        firstAlpha.insert(item);
                }
                if (first[sym].find(EPSILON) == first[sym].end()) {
                    allEpsilon = false;
                    break;
                }
            }
            if (allEpsilon)
                firstAlpha.insert(EPSILON);
            size_t before = first[A].size();
            first[A].insert(firstAlpha.begin(), firstAlpha.end());
            if (first[A].size() != before)
                changed = true;
        }
    }
    return first;
}

// Compute the FOLLOW sets for all nonterminals.
map<string, set<string>> computeFollowSets(const vector<Production>& grammar, const map<string, set<string>> &first) {
    map<string, set<string>> follow;
    for (const auto &prod : grammar) {
        if (isNonTerminal(prod.lhs))
            follow[prod.lhs]; // default empty set
    }
    follow["<sql>"].insert("$");
    
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto &prod : grammar) {
            const string &A = prod.lhs;
            for (size_t i = 0; i < prod.rhs.size(); i++) {
                string B = prod.rhs[i];
                if (isNonTerminal(B)) {
                    size_t before = follow[B].size();
                    bool betaAllEpsilon = true;
                    for (size_t j = i + 1; j < prod.rhs.size(); j++) {
                        const string &betaSym = prod.rhs[j];
                        for (const auto &item : first.at(betaSym)) {
                            if (item != EPSILON)
                                follow[B].insert(item);
                        }
                        if (first.at(betaSym).find(EPSILON) == first.at(betaSym).end()) {
                            betaAllEpsilon = false;
                            break;
                        }
                    }
                    if (betaAllEpsilon) {
                        follow[B].insert(follow[A].begin(), follow[A].end());
                    }
                    if (follow[B].size() != before)
                        changed = true;
                }
            }
        }
    }
    return follow;
}

// Construct the LL(1) parsing table.
map<pair<string, string>, vector<int>> constructParsingTable(const vector<Production>& grammar,
                                                              const map<string, set<string>> &first,
                                                              const map<string, set<string>> &follow) {
    map<pair<string, string>, vector<int>> table;
    for (int i = 0; i < grammar.size(); i++) {
        const Production &prod = grammar[i];
        set<string> firstAlpha;
        bool allEpsilon = true;
        for (const auto &sym : prod.rhs) {
            for (const auto &item : first.at(sym)) {
                if (item != EPSILON)
                    firstAlpha.insert(item);
            }
            if (first.at(sym).find(EPSILON) == first.at(sym).end()) {
                allEpsilon = false;
                break;
            }
        }
        if (allEpsilon)
            firstAlpha.insert(EPSILON);
        for (const auto &term : firstAlpha) {
            if (term != EPSILON) {
                table[{prod.lhs, term}].push_back(i);
            }
        }
        if (firstAlpha.find(EPSILON) != firstAlpha.end()) {
            for (const auto &b : follow.at(prod.lhs)) {
                table[{prod.lhs, b}].push_back(i);
            }
        }
    }
    return table;
}
