import json

from dataclasses import dataclass

import numpy as np

from .system import System


@dataclass
class Parameters:
    algorithm: str
    tolerance: float | None = None
    max_iters: int | None = None
    relaxation_factor: float | None = None

    @classmethod
    def from_dict(cls, j):
        return cls(
            j['algorithm'],
            j.get('iter_settings', {}).get('tolerance', None),
            j.get('iter_settings', {}).get('max_iter', None),
            j.get('relaxation_factor', None)
        )

    def to_dict(self):
        d = {
            'algorithm': self.algorithm,
        }

        if self.algorithm != 'lup':
            d['iter_settings'] = {
                'tolerance': self.tolerance,
                'max_iter': self.max_iters,
            }

            if self.algorithm == 'sor':
                d['relaxation_factor'] = self.relaxation_factor

        return d

    def to_json(self, filename: str):
        with open(filename, 'w') as f:
            json.dump(self.to_dict(), f)


@dataclass
class Input:
    params: Parameters
    system: System

    def to_dict(self):
        return {
            'params': self.params.to_dict(),
            'problem': self.system.to_dict(),
        }

    def to_json(self, filename: str):
        with open(filename, 'w') as f:
            json.dump(self.to_dict(), f)


@dataclass
class Solution:
    system: System
    params: Parameters
    flux: np.ndarray
    residual_error: float
    time: int
    memory: int | None = None

    converged: bool | None = None
    relative_error: float | None = None
    iterations: int | None = None

    @classmethod
    def from_dict(cls, data) -> 'Solution':
        return cls(
            System.from_dict(data['project']['problem']),
            Parameters.from_dict(data['project']['params']),
            np.asarray(data['flux']['m_data']).reshape(data['flux']['m_rows'], data['flux']['m_cols']),
            data['residual_error'],
            data['time'],

            converged=data.get('converged'),
            relative_error=data.get('relative_error'),
            iterations=data.get('iterations'),
        )

    @classmethod
    def from_json(cls, filename) -> 'Solution':
        with open(filename, 'r') as f:
            j = json.load(f)

        return cls.from_dict(j)