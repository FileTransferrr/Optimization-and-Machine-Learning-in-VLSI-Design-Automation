#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <cmath>
using namespace std;

enum Formula_State{
    Satisfied, Unsatisfied, Unsolved, Completed // States of the formula, 'completed' represents the process is completed successfully.
};

class CNF{
public:
    vector<int> literals;   // All the literals are stored in a vector
    vector<int> literal_appearance; // Record of the literals appearing in the formula
    vector<int> literal_polarity; // Record of the polarity of each literal(positive:1, negative:-1)
    vector<vector<int> > clause; // Array of the clauses

    CNF() {}
    CNF(const CNF& other){  // Assignment
        literals = other.literals;
        literal_appearance = other.literal_appearance;
        literal_polarity = other.literal_polarity;
        clause = other.clause;
    }

};

class DPLL_Algorithm{
private:
    CNF cnf;
    int literal_num;    // Denote the total sum of the literals
    int clause_num;     // Denote the total sum of the clauses
    int unit_propagate(CNF &);
    int DPLL(CNF &);
    int assignment(int, CNF &);
    void calculate_Formula_State(int, CNF &);
public:
    DPLL_Algorithm() {}
    void initialize();
    void solve();
};

void DPLL_Algorithm::initialize(){  // Get the clauses and literals from the input.cnf file
    char c;
    string s;

    while(true){
        cin >> c;
        if (c == 'c'){
            getline(cin, s);    // Ignore the first line of input
        }
        else{
            cin >> s;   // c equals to 'p' and ignore the next string
            break;
        }
    }

    cin >> literal_num >> clause_num;   // Update the vectors
    cnf.literals.clear();
    cnf.literals.resize(literal_num, -1);
    cnf.clause.clear();
    cnf.clause.resize(clause_num);
    cnf.literal_appearance.clear();
    cnf.literal_appearance.resize(literal_num, 0);
    cnf.literal_polarity.clear();
    cnf.literal_polarity.resize(literal_num, 0);

    int literal;
    for (int i = 0; i < clause_num; ++i){
        while(true){
            cin >> literal; // The literal in each clause
            if (literal > 0){   // The polarit of the input literal is postive
                cnf.clause[i].push_back(2*(literal-1)); // The literal is added to the i-th clause, and the literal is denoted by 2*(literal-1) if literal is positive
                cnf.literal_appearance[literal-1]++;    // The appearance of the literal is added by 1
                cnf.literal_polarity[literal-1]++;  // The polarity of the literal is added by 1
            }
            else if (literal < 0){  // The polarity of the input literal is negative
                cnf.clause[i].push_back(-2*literal-1);  // The literal is added to the i-th clause, and the literal is denoted by -2*literal-1 if literal is negative
                cnf.literal_appearance[-literal-1]++;   // The appearance of the literal is added by 1
                cnf.literal_polarity[-literal-1]--;     // The polarity of the literal is decreased by 1
            }
            else{
                break;  // The literal read in is 0
            }
        }
    }
}

int DPLL_Algorithm::unit_propagate(CNF & cnf){ // The first step of DPLL algorithm is attempting to propagate a clause into a unit literal form.
    bool unit_clause = false; // Initialize the unit_clause as false
    if (cnf.clause.size() == 0){ // No literal left in the clause and thus the clause is satisfied
        return Formula_State::Satisfied;
    }
    do{
        unit_clause = false;
        for (int i = 0; i < cnf.clause.size(); ++i){
            if (cnf.clause[i].size() == 1){
                unit_clause = true;
                cnf.literals[cnf.clause[i][0]/2] = cnf.clause[i][0] % 2; // 1 if negative, 0 if positive
                cnf.literal_appearance[cnf.clause[i][0]/2] = -1; // Remove the i-th literal from all the clauses
                int Formula_State = assignment(cnf.clause[i][0]/2, cnf); // Do the assignment
                if (Formula_State == Formula_State::Satisfied || Formula_State == Formula_State::Unsatisfied){  // This demonstrates that the problem is solved
                    return Formula_State;
                }
                break;
            }
            else if (cnf.clause[i].size() == 0){    // No literal in the i-th clause and UNSAT can be judged
                return Formula_State::Unsatisfied;
            }
        }
    } while(unit_clause);   // Recursively check whether the clause is unit.
    return Formula_State::Unsolved;
}

int DPLL_Algorithm::assignment(int assignment_literal, CNF & cnf){  // The second step of DPLL algorithm
    int value = cnf.literals[assignment_literal];
    for (int i = 0; i < cnf.clause.size(); ++i){
        for (int j = 0; j < cnf.clause[i].size(); ++j){
            if (2 * assignment_literal + value == cnf.clause[i][j]){    // positive: assign the literal with 1(value equals to clause[i][0]%2, assignment_literal equals to clause[i][0]/2)
                cnf.clause.erase(cnf.clause.begin()+i); //erase the clause from the vector because if one of the literals in this clause is assigned to 1, the this clause is SAT(satisfied)
                i--;    // Update the index
                if (cnf.clause.size() == 0){ // All the clauses are successfully processed and come out satisfied
                    return Formula_State::Satisfied;
                }
                break;
            }
            else if (cnf.clause[i][j]/2 == assignment_literal){ // negative: assign the literal with 0
                cnf.clause[i].erase(cnf.clause[i].begin()+j); // Now remove the 0-assigned literal from all the clauses
                j--;    // Update the index
                if (cnf.clause[i].size() == 0){ // After the assignment, no literal left in some clauses, then the conflict happens and comes out UNSAT(unsatisfied)
                    return Formula_State::Unsatisfied;
                }
                break;
            }
        }
    }
    return Formula_State::Unsolved; // Cannot judge from SAT or UNSAT, the state of the clauses is unsolved and another literal should be selected to be assigned.
}

int DPLL_Algorithm::DPLL(CNF & cnf){
    int Formula_State = unit_propagate(cnf);
    if (Formula_State == Formula_State::Satisfied){
        calculate_Formula_State(Formula_State, cnf);
        return Formula_State::Completed;    // The solution is completed and the result is SAT
    }
    else if (Formula_State == Formula_State::Unsatisfied){  // Current assignment ended with UNSAT and another set of assignments should be applied, so the state of the problem is unsolved
        return Formula_State::Unsolved;
    }

    int i = distance(cnf.literal_appearance.begin(), max_element(cnf.literal_appearance.begin(), cnf.literal_appearance.end())); // find the literal which appears most frequently
    for (int j = 0; j < 2; ++j){    // j is 0 or 1
        CNF new_cnf = cnf;  // copy the cnf in order to process and trace back
        if (new_cnf.literal_polarity[i] > 0){   // As for the i-th literal, it appears as positive variable more frequently than that of negative
            new_cnf.literals[i] = j;    // assign the i-th literal with j
        }
        else{
            new_cnf.literals[i] = (j+1) % 2; // negative more frequently, then assign the i-th literal with (j+1)%2
        }
        new_cnf.literal_appearance[i] = -1; // Remove the literal which has been already assigned
        int new_Formula_State = assignment(i, new_cnf); // Check whether this assignment can make the problem SAT
        if (new_Formula_State == Formula_State::Satisfied){
            calculate_Formula_State(new_Formula_State, new_cnf);    // SAT, then show the assignments of all the literals that make the problem satisfied
            return Formula_State::Completed;
        }
        else if (Formula_State == Formula_State::Unsatisfied){  // UNSAT, then apply the DPLL algorithm recursively
            continue;
        }

        int dpll_Formula_State = DPLL(new_cnf); // next cycle
        if (dpll_Formula_State == Formula_State::Completed){    // The problem is completed
            return dpll_Formula_State;
        }
    }
    return Formula_State::Unsolved; // The problem is unsolved and needs further decision and deduction
}

void DPLL_Algorithm::calculate_Formula_State(int Formula_State, CNF & cnf){ // Show the solution
    if (Formula_State == Formula_State::Satisfied){
        cout << "SAT" << endl;
        for (int i = 0; i < cnf.literals.size(); ++i){
            if (i != 0){
                cout << " ";
            }
            if (cnf.literals[i] != -1){
                if (cnf.literals[i] % 2 == 0){
                    cout << i+1;
                }
                else if (cnf.literals[i] % 2 == 1){
                    cout << -1 * (i+1);
                }
            }
            else{
                cout << i+1;
            }
        }
        cout << " 0" << endl;   // End with 0
    }
    else{
        cout << "UNSAT" << endl;
    }
}

void DPLL_Algorithm::solve(){
    int Formula_State = DPLL(cnf);
    if (Formula_State == Formula_State::Unsolved){  // If the problem is unsolved, then apply DPLL algorithm recursively 
        calculate_Formula_State(Formula_State::Unsatisfied, cnf);
    }
}

int main(){
    DPLL_Algorithm SAT_Solver;
    SAT_Solver.initialize();
    SAT_Solver.solve();
    return 0;
}
