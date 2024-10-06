import sys

def parse_dimacs_cnf(cnf_file):
    """
    解析 DIMACS 格式的 CNF 文件，返回子句列表
    """
    cnf = []
    with open(cnf_file, 'r') as file:
        for line in file:
            # 忽略注释和头部信息
            if line.startswith('p') or line.startswith('c'):
                continue
            # 将每个子句存储为整数列表，并且忽略子句末尾的 0
            clause = [int(x) for x in line.strip().split() if int(x) != 0]
            if clause:
                cnf.append(clause)
    return cnf

def parse_solution(solution_file):
    """
    解析解文件，返回 SAT 或 UNSAT 状态，以及变量的布尔值映射
    """
    solution = {}
    with open(solution_file, 'r') as file:
        first_line = file.readline().strip()

        # 检查第一行是否为 "SAT" 或 "UNSAT"
        if first_line == "UNSAT":
            print(f"The problem is unsatisfiable according to {solution_file}.")
            return None
        elif first_line != "SAT":
            raise ValueError(f"Invalid solution file format: {first_line}")

        # 解析第二行，包含每个变量的赋值
        second_line = file.readline().strip().split()
        for val in second_line:
            var = int(val)
            if var > 0:
                solution[var] = True
            else:
                solution[-var] = False

    return solution

def verify_solution(cnf, solution):
    """
    验证给定的解是否满足 CNF 表达式
    """
    for clause in cnf:
        satisfied = False
        for literal in clause:
            var = abs(literal)
            # 检查该子句是否满足
            if (literal > 0 and solution.get(var, False)) or (literal < 0 and not solution.get(var, False)):
                satisfied = True
                break
        # 如果某个子句不满足，则解不正确
        if not satisfied:
            return False
    return True

def main(cnf_file, solution_file):
    # 解析 CNF 文件
    cnf = parse_dimacs_cnf(cnf_file)
    
    # 解析解文件
    solution = parse_solution(solution_file)
    
    # 如果是 UNSAT，直接返回
    if solution is None:
        return
    
    # 验证解是否正确
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