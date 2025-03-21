import json

import numpy as np

from dataclasses import dataclass, field

from .special import *


__all__ = [
    "Grid",
    "System",
]


@dataclass
class Grid:
    a: float = 1
    b: float = 1
    M: int = 10
    N: int = 10

    def __post_init__(self) -> None:
        if self.a < 0:
            raise ValueError(f'`a` must be positive: {self.a} < 0')

        if self.b < 0:
            raise ValueError(f'`b` must be positive: {self.b} < 0')

        if self.M < 0:
            raise ValueError(f'`M` must be positive: {self.M} < 0')

        if self.N < 0:
            raise ValueError(f'`N` must be positive: {self.N} < 0')

    def get_mesh(self) -> tuple[np.ndarray, np.ndarray]:
        x = np.linspace(0, self.a, self.M + 2)[1:-1]
        y = np.linspace(0, self.b, self.N + 2)[1:-1]
        return np.meshgrid(x, y)

    @classmethod
    def build_square(cls, l: float = 1.0, n: int = 10) -> 'Grid':
        return cls(l, l, n, n)

    @property
    def shape(self) -> tuple[int, int]:
        return (self.M, self.N)

    @property
    def size(self) -> int:
        return np.prod(self.shape)

    @classmethod
    def from_json(cls, filename) -> 'Grid':
        with open(filename, 'r') as f:
            j = json.load(f)

        return cls.from_dict(j)

    @classmethod
    def from_dict(cls, j):
        return cls(
            j['space']['X'],
            j['space']['Y'],
            j['points']['NX'],
            j['points']['NY'],
        )

    def to_str(self) -> str:
        return f"{self.a:14.8e} {self.b:14.8e}\n{self.M} {self.N}"

    def to_dict(self):
        return {
            'space': {'X': self.a, 'Y': self.b},
            'points': {'NX': self.M, 'NY': self.N},
        }

    def to_json(self, filename: str):
        with open(filename, 'w') as f:
            json.dump(self.to_dict(), f)


@dataclass
class System:
    grid: Grid = field(default_factory=Grid)
    D: float = 1
    S: float = 1
    source: np.ndarray | None = None

    def __post_init__(self):
        if self.source is None:
            self.source = np.zeros(self.grid.shape)

        if self.source.shape != self.grid.shape:
            raise ValueError(f'grid and source shape must match: {self.grid.shape} != {self.source.shape}')

        if self.D < 0:
            raise ValueError(f'`D` must be positive: {self.D} < 0')

        if self.S < 0:
            raise ValueError(f'`S` must be positive: {self.S} < 0')

        if np.any(self.source < 0):
            raise ValueError('source must be non-negative')

    @classmethod
    def from_point_sources(cls, grid,
                           locs: tuple[float, float] | list[tuple[float, float]],
                           fwhms: float | list[float] = 1e-2,
                           qs: float | list[float] = 1.0,
                           kernel=gaussian_2d, **kwargs) -> 'Problem':
        x, y = grid.get_mesh()
        source = np.zeros_like(x)

        locs = locs if not isinstance(locs, tuple) else [locs]
        fwhms = fwhms if not isinstance(fwhms, float) else [fwhms] * len(locs)
        qs = qs if not isinstance(qs, float) else [qs] * len(locs)

        for (x0, y0), fwhm, q in zip(locs, fwhms, qs):
            source += q * kernel(x, y, x0=x0, y0=y0, fwhm=fwhm)

        return cls(grid, source=source, **kwargs)

    @classmethod
    def from_constant_source(cls, grid, q=1.0, **kwargs):
        source = np.full((grid.M, grid.N), q)
        return cls(grid, source=source, **kwargs)

    @classmethod
    def from_line_source(cls, grid, a = 0.0, b=0.5, q=1.0, distance=1e-2, **kwargs) -> 'System':
        x, y = grid.get_mesh()
        source = np.zeros_like(x)
        d = np.abs(y - a * x - b) / np.sqrt(a ** 2 + 1)
        source[d < distance] = q
        return cls(grid, source=source, **kwargs)

    @classmethod
    def from_json(cls, filename) -> 'System':
        with open(filename, 'r') as f:
            j = json.load(f)

        return cls.from_dict(j)

    @classmethod
    def from_dict(cls, j):
        return cls(
            Grid.from_dict(j['grid']),
            j['absorption_scattering'],
            j['diffusion_coefficient'],
            np.asarray(j['source']['m_data']).reshape(j['source']['m_rows'], j['source']['m_cols'])
        )

    def to_dict(self):
        d = {
            'grid': grid.to_dict(),
            'absorption_scattering': self.D,
            'diffusion_coefficient': self.S,
            'source': {
                'm_rows': self.source.shape[0],
                'm_cols': self.source.shape[1],
                'm_data': self.source.flatten().tolist()
            }
        }

        return d

    def to_str(self) -> str:
        rows = [
            self.grid.to_str(),
            f'{self.D:14.8e} {self.S:14.8e}',
            ''
        ]

        for row in self.source:
            rows.append(' '.join(map(lambda v: f'{v:14.8e}', row)))

        return '\n'.join(rows)

    def to_json(self, filename: str):
        with open(filename, 'w') as f:
            json.dump(self.to_dict(), f)