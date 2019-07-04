# K-Means Clustering for Sequential Data

A somewhat parallel implementation (using OpenMP) of [The SKM Algorithm: A K-Means Algorithm for Clustering Sequential Data](https://link.springer.com/chapter/10.1007/978-3-540-88309-8_18) for the purpose of creating language bindings.

## Testing
```bash
make test
./bin/test
```

To customize the data that is used, open `src/test.c` to modify the data that is used by `sequential_kmeans`.


## Python
The implementation now includes Python bindings.  The Python method signature is as follows:
```python
skm(list sequences, int k, int max_iterations, int verbose=0)
```

### Installation
To use/install the implementation from Python, execute in the following bash command to build the bindings with `Cython` and install the module:
```bash
make python
```
