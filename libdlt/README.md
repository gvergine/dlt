# libdlt

## Compile

### Debug with code coverage
```
~/dlt/libdlt_build_debug$ cmake -DCMAKE_BUILD_TYPE=Debug ../libdlt && make && make test && ctest -T memcheck && make libdlt_coverage
```

### Release
```
~/dlt/libdlt_build_release$ cmake -DCMAKE_BUILD_TYPE=Release ../libdlt && make && make test && ctest -T memcheck
```
