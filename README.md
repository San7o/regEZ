<h1 align=center>regEZ</h1>

regEZ is a modern, header only implementation of a general regex engine in C++23. It works with any container using an user-defined grammar.

## Supported grammar

The regex will allow the following tokens:

- `regez_concat`: concatenation operator

- `regez_or`: "objectA OR objectB" matches either "objectA" or "objectB"

- `regez_any`: "objectA ANY" matches any number of occurences of "objectA", even 0

- `regez_open_group`: opens a new group, eg. "("

- `regez_close_group`: closes a group, eg. ")"
    
- `regez_open_match`, `regez_close_match`: the regex will match any object encosed
beween `open_match` and `closed_match`. eg. with `open_match`="[" and `closed_match`="]", 
"[ABC]" will match either A, or B, or C
    
- `regez_one_or_more`: "+"

- `regez_escape`: escape any of the previous tokens

## Works with any type!

Yes, this does not work only for strings, but for any container. The tokens 
are of the same type as the regex and can be assigned when creating the regex object.
This means what you could make some efficient pattern matching and queries on
exotic datasets.

This is an example with the usual string regex syntx we all know:
```c++
// Define the regex grammar
auto voc = regez::Vocabulary()
    .set(regez::regez_open_group, '(');
    .set(regez::regez_close_group, ')');
    .set(regez::regez_open_match, '[');
    .set(regez::regez_close_match, ']');
    .set(regez::regez_or, '|');
    .set(regez::regez_any, '*');
    .set(regez::regez_concat, '.');
    .setn(regez::regez_escape, '\\');

// Create a regex object
regez::regex<std::string> r(std::string("(a|b)*c"), voc);
```

## Json serialization

The regex DFA can be serialized in a protable json format, so It can be easy ported 
elsewhere (handy for making visualizations for example).

```c++
regez::prettify(std::format("{}\n", my_regex)) 
```

```json
{
   "regex": {
     "start": {
       "state": {
         "id": 1,
         "is_final": false
      }
    },
     "states": [
      {
         "state": {
           "id": 2,
           "is_final": true
        }
      },
      {
         "state": {
           "id": 3,
           "is_final": false
        }
      }
    ]
  }
}
```


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

