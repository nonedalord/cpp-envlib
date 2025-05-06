cpp-envlib
===========

**A lightweight C++17 header-only environment variables library**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)  
![C++17](https://img.shields.io/badge/C%2B%2B-17-%2300599C)  
![Linux](https://img.shields.io/badge/Platform-Linux-lightgrey)

## Features
- **Zero-dependencies** — Requires only C++17 standard library
- **Header-only** — Single-file integration via `libenv.h`
- **Flexible error handling** — Exceptions and `noexcept` methods
- **Core type support** — `int`, `bool`, `string`, `double`, `long long`

Usage
---------------

### Initialization and Reading Values

```c++
env_cfg::EnvMap config = {  
    {"PORT", env_cfg::EnvCfgTypes::int_}, // Enforce type  
    {"HOST", "localhost"},                // Default value  
    {"DEBUG_MODE", true}                  // Boolean type  
};  

env_cfg::EnvCfg env;  
env.InitEnv(config);  

// Reading with error handling  
try 
{  
    int port = env.Get<int>("PORT"); // Throws on missing key/type mismatch  
    std::string host = env.Get<std::string>("HOST");  
} 
catch (const env_cfg::EnvBadGet& e) 
{  
    std::cerr << "Error: " << e.what() << std::endl;  
}  

// Safe access (noexcept)  
std::optional<bool> debug = env.GetN<bool>("DEBUG_MODE");  
if (debug) 
{  
    std::cout << "Debug mode: " << *debug << std::endl;  
}  

```

### Direct Environment Access

```c++
// Read with exception handling  
try
{
    int port = env_cfg::EnvCfg::GetW<int>("PORT"); 
}
catch (const env_cfg::EnvBadGet& e)
{
    std::cerr << "Error: " << e.what() << std::endl;  
}
// Read without exceptions  
int timeout = env_cfg::EnvCfg::GetW<int>("TIMEOUT").default_value(30);  
```

### Setting Environment Variables

```c++
// With name validation and overwrite 
try
{
    env_cfg::EnvCfg::SetEnv("API_KEY", "secret123", true);
}
catch (const env_cfg::EnvSetError& e)
{
    std::cerr << "Error: " << e.what() << std::endl;  
}

// No-throw setup  
bool success = env_cfg::EnvCfg::SetEnvN("LOG_LEVEL", "debug", false);  
if (!success) 
{  
    std::cerr << "Failed to set variable" << std::endl;  
}  
```
### Iterating Over Data

```c++
for (const auto& [key, value] : env) 
{  
    std::cout << key << " = " << value << std::endl;  
}  
```

## API Documentation

### Core Methods

#### Initialization & Reading
| Method | Description |
|--------|-------------|
| **`InitEnv(EnvMap)`** | Initializes environment variables using a key-type/default value map.<br>**Throws:** `EnvException` on parsing or system errors. |
| **`Get<T>(key)`** | Returns a value of type `T` for the specified key.<br>**Throws:** `EnvBadGet` if:<br>- Key is missing<br>- Type mismatch<br>- Value is empty |
| **`GetN<T>(key)`** | Safe version that never throws (`noexcept`). <br>Returns: `std::optional<T>`. |
| **`GetW<T>(key)`** | Directly reads from system environment (bypasses initialization). Returns `EnvDefaultValue<T>` wrapper:<br>- `.default_value(defaultvalue)` - returns value or defaultvalue<br>- `operator T()` - throws `EnvBadGet` on errors |

#### Environment Modification
| Method | Description |
|--------|-------------|
| **`SetEnv(name, value)`** | Sets an environment variable with validation.<br>- Checks for valid names (no `=` or empty)<br>**Throws:** `EnvSetError` on invalid names or `setenv` failures |
| **`SetEnvN(name, value)`** | No-throw version of `SetEnv`. Returns `true` on success, `false` on failure (`noexcept`). |

#### Validation & Checks
| Method | Description |
|--------|-------------|
| **`HasValue(key)`** | Checks if a key exists and has a non-empty value (`noexcept`).<br>Returns: `bool` |
| **`IsType<T>(key)`** | Verifies that the stored value exactly matches type `T` (`noexcept`).<br>Returns: `bool` |

License
-------

MIT license (© 2025 Daniil Aleev)