# Authon C++ SDK

<p align="center">
  <img src="https://authon.pro/logo.png" alt="Authon" width="80" />
  <br/>
  <strong>Official C++ SDK for Authon — Software Licensing & Authentication Platform</strong>
</p>

<p align="center">
  <a href="https://authon.pro">Website</a> •
  <a href="https://authon.pro/docs">Docs</a> •
  <a href="https://discord.gg/jMZCTKPsmE">Discord</a> •
  <a href="https://authon.pro/status">Status</a>
</p>

---

## Requirements

- Windows (uses WinHTTP)
- C++17 or later
- Link with: `winhttp.lib` (MSVC) or `-lwinhttp` (MinGW)

## Installation

Single header — just copy `authon.h` into your project:

```cpp
#include "authon.h"
```

## Compile

**MSVC (Visual Studio):**
```bash
cl /EHsc /O2 example.cpp winhttp.lib
```

**MinGW:**
```bash
g++ -O2 -o example.exe example.cpp -lwinhttp
```

## Quick Start

```cpp
#include "authon.h"

int main() {
    authon::Authon auth("your-app-id", "your-api-key");

    if (!auth.init()) {
        printf("Connection failed\n");
        return 1;
    }

    if (auth.login("username", "password")) {
        printf("Level: %d\n", auth.level);
        printf("Expires: %s\n", auth.expiresAt.c_str());
    } else {
        printf("Error: %s\n", auth.lastError.c_str());
    }

    auth.logout();
    return 0;
}
```

## Authentication

### Login
```cpp
if (auth.login("user", "pass")) {
    // auth.level, auth.subscription, auth.expiresAt, auth.sessionToken
}
```

### License Key
```cpp
if (auth.license("XXXXX-XXXXX-XXXXX-XXXXX")) {
    printf("Valid! Level: %d\n", auth.level);
}
```

### Register
```cpp
if (auth.registerUser("newuser", "password", "LICENSE-KEY")) {
    printf("Account created!\n");
}
```

## Features

```cpp
// Variables
std::string val = auth.getVar("key");
auth.setVar("key", "value");
std::string uvar = auth.getUserVar("key");

// File download (raw bytes)
auto data = auth.downloadFile("file-id");
// data is std::vector<unsigned char>

// Activity log
auth.log("User opened application");

// Session check
bool valid = auth.check();

// Logout
auth.logout();
```

## HWID

Auto-generated from system hardware. Override:
```cpp
auth.login("user", "pass", "custom-hwid");
```

## Links

- 🌐 Website: https://authon.pro
- 📖 Docs: https://authon.pro/docs
- 💬 Discord: https://discord.gg/jMZCTKPsmE
- 📊 Status: https://authon.pro/status
- 🔗 API Health: https://api.authon.pro/health

## License

MIT
