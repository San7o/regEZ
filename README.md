<h1 align=center>regEZ</h1>

regEZ is a modern, header only implementation of a general regex engine in C++23. It works with any container with custom tokens, fully constexpr compatible!

## Works with any type!

Yes, this does not work only for strings, but for any container.
This means what you could make some efficient pattern matching and queries on
exotic datasets.

This is an example with the usual string regex syntx we all know and love:
```c++
// Define the regex vocabulary
auto vocab = regez::Vocabulary()
    .set(regez::regez_open_group, '(')
    .set(regez::regez_close_group, ')')
    .set(regez::regez_open_match, '[')
    .set(regez::regez_close_match, ']')
    .set(regez::regez_or, '|')
    .set(regez::regez_any, '*')
    .set(regez::regez_concat, '.')
    .set(regez::regez_escape, '\\');

// Create a regex object
regez::Regex<std::string> r(std::string("(a|b)*c"), vocab);
// Match the regex to a string
bool is_matched = regez.match(std::string("Nice"));
```

## Supported tokens

The regex will allow the following tokens:

- `regez_concat`: concatenation operator

- `regez_or`: "objectA OR objectB" matches either "objectA" or "objectB"

- `regez_any`: "objectA ANY" matches any number of occurences of "objectA", even 0

- `regez_open_group`: opens a new group, eg. "("

- `regez_close_group`: closes a group, eg. ")"
    
- `regez_one_or_more`: "+"

- `regez_escape`: escape any of the previous tokens

## Current State

The library is currently in developement and It's not intended for production use.
If you want to contribute, check out the issues!

## Documentation

TODO

## Testing

To build the tests, run cmake with `REGEZ_BUILD_TESTS=ON` option.
```bash
cmake -B build -D REGEZ_BUILD_TESTS=ON
cmake --build build
```

## License

The project falls under [MIT](./LICENSE) license.

