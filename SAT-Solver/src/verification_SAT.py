import sys

def parse_dimacs_cnf(cnf_file):
    cnf = []
    with open(cnf_file, 'r') as file:
        for line in file:
            if line.startswith('p') or line.startswith('c'):
                continue
            clause = [int(x) for x in line.strip().split() if int(x) != 0]
            if clause:
                cnf.append(clause)
    return cnf

def parse_solution(solution_file):
    solution = {}
    with open(solution_file, 'r') as file:
        first_line = file.readline().strip()

        if first_line == "UNSAT":
            print(f"The problem is unsatisfiable according to {solution_file}.")
            return None
        elif first_line != "SAT":
            raise ValueError(f"Invalid solution file format: {first_line}")

        second_line = file.readline().strip().split()
        for val in second_line:
            var = int(val)
            if var > 0:
                solution[var] = True
            else:
                solution[-var] = False

    return solution

def verify_solution(cnf, solution):
    for clause in cnf:
        satisfied = False
        for literal in clause:
            var = abs(literal)
            if (literal > 0 and solution.get(var, False)) or (literal < 0 and not solution.get(var, False)):
                satisfied = True
                break
        if not satisfied:
            return False
    return True

def main(cnf_file, solution_file):
    cnf = parse_dimacs_cnf(cnf_file)
    solution = parse_solution(solution_file)
    
    if solution is None:
        return
    
    if verify_solution(cnf, solution):
        print(f"The solution in {solution_file} is correct.")
    else:
        print(f"The solution in {solution_file} is incorrect.")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 sat_verify.py <cnf_file> <solution_file>")
        sys.exit(1)

    cnf_file = sys.argv[1]
    solution_file = sys.argv[2]

    main(cnf_file, solution_file)
