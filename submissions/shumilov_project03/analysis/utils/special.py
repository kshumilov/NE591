import numpy as np
import numpy.typing as npt


__all__ = [
    "gaussian_2d",
    "lorentzian_2d",
]


def gaussian_2d(x: npt.ArrayLike, y: npt.ArrayLike, x0: float = 0.5, y0: float = 0.5, fwhm: float = 1.0) -> np.ndarray:
    sigma = fwhm / (2 * np.sqrt(2 * np.log(2)))
    return np.exp(-((x - x0) ** 2 + (y - y0) ** 2) / (2 * sigma ** 2)) / (2 * np.pi * sigma ** 2)


def lorentzian_2d(x: npt.ArrayLike, y: npt.ArrayLike,  x0: float = 0.5, y0: float = 0.5, fwhm: float = 1.0) -> np.ndarray:
    return fwhm / (2 * np.pi) / ((x - x0) ** 2 + (y - y0) ** 2 + (fwhm / 2) ** 2) ** 1.5
