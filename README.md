<h1 align=center>regEZ</h1>

regEZ is a modern, header only implementation of a general regex engine in C++23. It works with any type using an user-defined grammar.


## Documentation

TODO

## Testing

This library uses `Boost.Test` for testing. To build the tests, run cmake with `REGEZ_BUILD_TESTS=ON` option.
```bash
cmake -B build -D REGEZ_BUILD_TESTS=ON
cmake --build build
```

## License

The project falls under [MIT](./LICENSE) license.
