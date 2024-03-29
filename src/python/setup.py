from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

ext_modules = [
    Extension(
        "skm",
        ["src/python/skm.pyx"],
        extra_compile_args=['-fopenmp'],
        extra_link_args=['-fopenmp'],
    ),
    Extension(
        "markov",
        ["src/python/markov.pyx"],
        extra_compile_args=['-fopenmp'],
        extra_link_args=['-fopenmp'],
    )
]


setup(
    name='skm',
    ext_modules=cythonize(ext_modules)
)
