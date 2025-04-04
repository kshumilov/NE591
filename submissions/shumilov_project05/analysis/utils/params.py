import json

from dataclasses import dataclass


__all__ = [
    "Parameters",
]

LUP = "lup"
PJ = "pj"
GS = "gs"
SOR = "sor"

ALGO_INT_TO_STR = [LUP, PJ, GS, SOR]
ALGO_STR_TO_INT = {a: i for i, a in enumerate(ALGO_INT_TO_STR)}


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

        if self.algorithm != LUP:
            d['iter_settings'] = {
                'tolerance': self.tolerance,
                'max_iter': self.max_iters,
            }

            if self.algorithm == SOR:
                d['relaxation_factor'] = self.relaxation_factor

        return d

    def to_str(self) -> str:
        rows = [
            str(ALGO_STR_TO_INT[self.algorithm])
        ]

        if self.algorithm != LUP:
            rows.extend([
                '',
                f'{self.max_iters:d} {self.tolerance:14.8e}'
            ]) 

        if self.algorithm == SOR:
            rows.extend([
                f'{self.relaxation_factor:14.8e}'
            ])

        return '\n'.join(rows)
 
    def to_json(self, filename: str):
        with open(filename, 'w') as f:
            json.dump(self.to_dict(), f)


