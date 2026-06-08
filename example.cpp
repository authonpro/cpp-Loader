/*
 * Authon C++ SDK - Full Usage Example
 * Compile: g++ -O2 -o example.exe example.cpp -lwinhttp
 *   or: cl example.cpp winhttp.lib
 */

#include <iostream>
#include <string>
#include <conio.h>
#include "authon.h"

int main() {
    // ============ SETUP ============
    authon::Authon auth("your-app-id", "your-api-key");

    // ============ CONNECT ============
    std::cout << "[*] Connecting...\n";
    if (!auth.init()) {
        std::cout << "[-] Failed to connect to Authon API\n";
        return 1;
    }
    std::cout << "[+] Connected: " << auth.appName << " v" << auth.appVersion << "\n";

    // ============ AUTHENTICATE ============
    std::cout << "\n[1] Login (Username + Password)\n";
    std::cout << "[2] License Key\n";
    std::cout << "\n> ";
    int choice = _getch() - '0';
    std::cout << choice << "\n\n";

    bool success = false;
    if (choice == 1) {
        std::string username, password;
        std::cout << "Username: ";
        std::getline(std::cin, username);
        std::cout << "Password: ";
        std::getline(std::cin, password);
        success = auth.login(username, password);
    } else {
        std::string key;
        std::cout << "License Key: ";
        std::getline(std::cin, key);
        success = auth.license(key);
    }

    if (!success) {
        std::cout << "\n[-] " << auth.lastError << "\n";
        system("pause");
        return 1;
    }

    std::cout << "\n[+] Authenticated!\n";
    std::cout << "    Level: " << auth.level << "\n";
    std::cout << "    Subscription: " << (auth.subscription.empty() ? "None" : auth.subscription) << "\n";
    std::cout << "    Expires: " << (auth.expiresAt.empty() ? "Lifetime" : auth.expiresAt) << "\n";

    // ============ USE FEATURES ============

    // Get variable
    std::string msg = auth.getVar("welcome_message");
    if (!msg.empty())
        std::cout << "\n[*] " << msg << "\n";

    // Log activity
    auth.log("C++ SDK example executed");

    // Session check
    if (auth.check())
        std::cout << "\n[+] Session is valid\n";

    // ============ CLEANUP ============
    std::cout << "\n[+] Done. Logging out...\n";
    auth.logout();

    system("pause");
    return 0;
}
