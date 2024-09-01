<h1 align=center>regEZ</h1>

regEZ is a modern, header only implementation of a general regex engine in C++23. It works with any type using an user-defined grammar.

## Supported grammar

The regex will allow the following tokens:

- `REGEZ_CONCAT`: concatenation operator

- `REGEZ_OR`: "objectA OR objectB" matches either "objectA" or "objectB"

- `REGEZ_ANY`: "objectA ANY" matches any number of occurences of "objectA", even 0

- `REGEZ_OPEN_GROUP`: opens a new group, eg. "("

- `REGEZ_CLOSE_GROUP`: closes a group, eg. ")"
    
- `REGEZ_OPEN_MATCH`, `REGEZ_CLOSE_MATCH`: the regex will match any object encosed
beween "OPEN_MATCH" and "CLOSED_MATCH". eg. with "OPEN_MATCH"="[" and "CLOSED_MATCH"="]", 
"[ABC]" will match either A, or B, or C
    
- `REGEZ_ESCAPE`: escape any of the previous tokens

## Works with any type!

Yes, this does not only work for string, but for any user defined types. The tokens 
are of the same type as the regex and can be assigned when creating the regex object.
This means what you could make some efficient pattern matching and queries on
exotic datasets.

This is an example with the usual string regex syntx we all know:
```bash
// Define the regex grammar
regez::grammar<std::string> g;
g.set_token(std::string("("), regez::REGEZ_OPEN_GROUP);
g.set_token(std::string(")"), regez::REGEZ_CLOSE_GROUP);
g.set_token(std::string("["), regez::REGEZ_OPEN_MATCH);
g.set_token(std::string("]"), regez::REGEZ_CLOSE_MATCH);
g.set_token(std::string("|"), regez::REGEZ_OR);
g.set_token(std::string("*"), regez::REGEZ_ANY);
g.set_token(std::string("."), regez::REGEZ_CONCAT);
g.set_token(std::string(R"(\\)"), regez::REGEZ_ESCAPE);

// Create a regex object
regez::regex<std::string> r(std::string("(a|b)*c"));
```

## Json serialization

The regex DFA can be serialized in a protable json format, so It can be easy ported 
elsewhere (nice to make visualizations for example).

```bash
regez::prettify(std::format("{}\n", my_regex)) 
```

## Current State

The library is currently in developement and It's not intended for production use.
If you want to contribute, check out the issues!

## Documentation

TODO

## Testing

This library uses `Boost.Test` for testing. To build the tests, run cmake with 
`REGEZ_BUILD_TESTS=ON` option.
```bash
cmake -B build -D REGEZ_BUILD_TESTS=ON
cmake --build build
```

## License

The project falls under [MIT](./LICENSE) license.
