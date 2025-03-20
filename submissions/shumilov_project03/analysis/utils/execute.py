import subprocess
import pathlib
import json

import pandas as pd
import numpy as np

from .solution import Solution, Parameters, Input
from .system import Grid, System



def parse_peak_memory(time_output: str) -> int:
    return int(time_output.strip().split('\n')[-1].strip().split()[0])


def run(executable: pathlib.Path, input_path: pathlib.Path, result_path: pathlib.Path) -> Solution:
    result = subprocess.run([
        'time', '-lha', executable, 
        input_path, '--input-json', 
        '-o', result_path, '--output-json'
    ],
    capture_output=True, text=True)

    solution = Solution.from_json(result_path)
    solution.memory = parse_peak_memory(result.stderr)
    return solution


def generate_data(
    executable: pathlib.Path, 
    param_sets: list[Parameters], 
    grid_sizes: list[int], 
    tests_dir: pathlib.Path
) -> tuple[pd.DataFrame, dict]:
    solutions = {}
    data = []
    
    input_filename = 'p{}_{}.json'
    result_filename = 'p{}_{}_result.json'

    for n in grid_size:
        grid = Grid.build_square(n=n)
        problem = System.from_point_sources(grid, (0.25, 0.25), fwhms=0.05, kernel=gaussian_2d)

        for params in param_sets:
            inp = Input(params, problem)

            input_path = tests_dir / input_filename.format(n, params.algorithm)
            result_path = tests_dir / result_filename.format(n, params.algorithm)
            inp.to_json(input_path)

            solutions[(n, params.algorithm)] = solution = run(
                executable,
                input_path,
                result_path,
            )

            #result = subprocess.run(['time', '-lha', executable, input_path, '--input-json', '-o', result_path, '--output-json'], 
            #                        capture_output=True, text=True)
            #solution = Solution.from_json(result_path)
            #solution.memory = parse_peak_memory(result.stderr)
            #solutions[(n, params.algorithm)] = solution

            data.append({
                'n': n,
                'algo': params.algorithm,
                'residual': solution.residual_error,
                'time': solution.time,
                'memory': solution.memory,
                'converged': bool(solution.converged),
                'iterations': solution.iterations,
                'relative': solution.relative_error,
                'relative_lup': np.abs(solutions[(n, 'lup')].flux - solution.flux).max()
            })

    df = pd.DataFrame.from_records(data)
    df['sec'] = df['time'] / 1e3
    df['kbytes'] = df['memory'] / 1024

    return df, data