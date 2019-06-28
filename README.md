# K-Means Clustering for Sequential Data

A somewhat parallel implementation of [The SKM Algorithm: A K-Means Algorithm for Clustering Sequential Data](https://link.springer.com/chapter/10.1007/978-3-540-88309-8_18) sing OpenMP for the purpose of creating language bindings.

## Testing
```
make
./bin/test
```

To customize the data that is used, open `src/test.c` to modify the data that is used by `sequential_kmeans`.
