import json

from dataclasses import dataclass

from .system import System
from .params import Parameters


__all__ = [
    "Inputs",
]


@dataclass
class Inputs:
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

    def to_str(self) -> str:
        return '\n'.join([
            self.params.to_str(),
            '',
            self.system.to_str(),
        ])

    def to_txt(self, filename) -> None:
        with open(filename, 'w') as f:
            f.write(self.to_str())
