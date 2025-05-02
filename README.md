cpp-envlib
===========

**A lightweight C++17 header-only environment variables library**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)  
![C++17](https://img.shields.io/badge/C%2B%2B-17-%2300599C)  
![Linux](https://img.shields.io/badge/Platform-Linux-lightgrey)

## Features ✨
- **Zero-dependencies** — Requires only C++17 standard library
- **Header-only** — Single-file integration via `libenv.h`
- **Flexible error handling** — Exceptions and `noexcept` methods
- **Core type support** — `int`, `bool`, `string`, `double`, `long long`

Usage
---------------

### Type-checked reading

```c++
env_cfg::EnvMap env_conf = {
    {"PORT", env_cfg::EnvCfgTypes::int_},
    {"HOST", "localhost"} // Default value
};

a.InitEnv(env_conf);
int port = a.Get<int>("PORT"); // Throws on type mismatch

```

### Default values

```c++
env_cfg::EnvMap config = {
    {"API_KEY", "default_secret"},
    {"TIMEOUT", 30}
};

a.InitEnv(config);
std::string key = a.Get<std::string>("API_KEY"); // Uses default_secret if env var missing
```

### Direct environment access

```c++
try 
{
    bool debug_mode = env_cfg::EnvCfg::GetW<bool>("DEBUG_MODE");
} 
catch(const std::exception& e) 
{
    std::cerr << "Error: " << e.what() << std::endl;
}
```

## API Documentation

### Core Methods

| Method                       | Behavior                                                                 |
|------------------------------|--------------------------------------------------------------------------|
| `InitEnv(EnvMap)`            | Initializes environment configuration using provided key-type/default map <br> Throw exception on errors.
| `Get<T>(key)`                | Returns value of type `T` for specified key. Throw exception on:<br>- Missing key<br>- Type mismatch<br>- Empty value |
| `GetP<T>(key)`               | Returns `std::unique_ptr<T>` containing value or `nullptr` if:<br>- Key missing<br>- Type mismatch<br>- Empty value<br>(noexcept) |
| `GetW<T>(key)`               | Directly reads environment variable from system (bypasses initialization). <br> Return value of type `T`. Throw exception if variable doesn't exist <br> |
| `GetWP<T>(key)`              | Directly reads environment variable from system (bypasses initialization)<br> Return `std::unique_ptr<T>` containing value or `nullptr` (noexcept)<br> |
| `HasValue(key)`              | Checks if key exists and contains non-nullopt value (noexcept)       |
| `IsType<T>(key)`             | Verifies that stored value exactly matches type `T` (noexcept)

License
-------

MIT license (© 2025 Daniil Aleev)