// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Authon C++ SDK — Software Licensing & Authentication                      ║
// ║  Version: 1.0.0                                                            ║
// ║  Single-header library (WinHTTP)                                           ║
// ║                                                                            ║
// ║  Website: https://authon.pro                                               ║
// ║  Docs:    https://authon.pro/docs                                          ║
// ║  Discord: https://discord.gg/jMZCTKPsmE                                    ║
// ║  Status:  https://authon.pro/status                                        ║
// ║  Health:  https://api.authon.pro/health                                    ║
// ║  GitHub:  https://github.com/authonpro                                     ║
// ║                                                                            ║
// ║  Requirements: Windows, WinHTTP (link winhttp.lib)                         ║
// ║  Compatible with: MSVC, MinGW (C++17 or later)                             ║
// ║                                                                            ║
// ║  Usage:                                                                    ║
// ║    #define AUTHON_IMPLEMENTATION                                            ║
// ║    #include "authon.h"                                                     ║
// ║                                                                            ║
// ║    Authon auth("app-id", "api-key");                                       ║
// ║    if (auth.init()) {                                                      ║
// ║        auto result = auth.login("user", "pass");                           ║
// ║        if (result.success) { /* authenticated */ }                         ║
// ║    }                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#pragma once
#ifndef AUTHON_H
#define AUTHON_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

// ═══════════════════════════════════════════════════════════════════════════════
// DATA STRUCTURES
// ═══════════════════════════════════════════════════════════════════════════════

namespace authon {

/// Represents an API response from Authon
struct Response {
    bool success = false;
    std::string message;
    std::map<std::string, std::string> data;
    std::vector<std::map<std::string, std::string>> dataArray;
    std::vector<unsigned char> binary;
};

/// Represents a file entry from list_files
struct FileInfo {
    std::string id;
    std::string name;
    std::string size;
    std::string minLevel;
};

/// Represents online users data
struct OnlineData {
    int count = 0;
    std::vector<std::string> users;
};

/// Represents application statistics
struct StatsData {
    int totalUsers = 0;
    int onlineUsers = 0;
    int totalKeys = 0;
    std::string appVersion;
};

} // namespace authon


// ═══════════════════════════════════════════════════════════════════════════════
// CLASS DECLARATION
// ═══════════════════════════════════════════════════════════════════════════════

namespace authon {

class Authon {
public:
    // ─── Constructor ─────────────────────────────────────────────────────────
    /// Creates a new Authon client instance.
    /// @param appId  Your Application ID from the Authon dashboard.
    /// @param apiKey Your API Key from the Authon dashboard (Settings tab).
    /// @param apiUrl Optional custom API base URL (default: https://api.authon.pro/v1).
    Authon(const std::string& appId, const std::string& apiKey,
           const std::string& apiUrl = "https://api.authon.pro/v1");

    ~Authon() = default;

    // ─── Initialization ─────────────────────────────────────────────────────
    /// Initialize connection and validate credentials.
    /// Must be called before any other API method.
    /// Populates appName, appVersion, hwidLock, hashCheck on success.
    /// @return true if initialization was successful.
    bool init();

    // ─── Authentication ─────────────────────────────────────────────────────
    /// Authenticate with username and password.
    /// On success, sets sessionToken, username, level, subscription, expiresAt.
    /// @param username User's username.
    /// @param password User's password.
    /// @param hwid     Optional HWID (auto-generated if empty).
    /// @return Response with success status and user data.
    Response login(const std::string& username, const std::string& password,
                   const std::string& hwid = "");

    /// Authenticate with a license key only.
    /// On success, sets sessionToken, level, subscription, expiresAt.
    /// @param licenseKey The license key to validate/activate.
    /// @param hwid       Optional HWID (auto-generated if empty).
    /// @return Response with success status and session data.
    Response license(const std::string& licenseKey, const std::string& hwid = "");

    /// Register a new user account with a license key.
    /// @param username   Desired username for the new account.
    /// @param password   Desired password.
    /// @param licenseKey A valid, unused license key.
    /// @param hwid       Optional HWID (auto-generated if empty).
    /// @return Response indicating success or failure.
    Response registerUser(const std::string& username, const std::string& password,
                          const std::string& licenseKey, const std::string& hwid = "");

    // ─── Session Management ─────────────────────────────────────────────────
    /// Validate current session (heartbeat).
    /// @return true if session is still valid.
    bool check();

    /// End the current session and clear local state.
    /// @return true if logout was successful.
    bool logout();

    // ─── Variables ──────────────────────────────────────────────────────────
    /// Get an application-level variable (shared across all users).
    /// @param key Variable name.
    /// @return Variable value, or empty string if not found.
    std::string getVar(const std::string& key);

    /// Set a user-level variable (stored per authenticated user).
    /// @param key   Variable name.
    /// @param value Variable value.
    /// @return true if saved successfully.
    bool setVar(const std::string& key, const std::string& value);

    /// Get a user-level variable.
    /// @param key Variable name.
    /// @return Variable value, or empty string if not found.
    std::string getUserVar(const std::string& key);

    // ─── Files ──────────────────────────────────────────────────────────────
    /// List files available for the current user's level.
    /// @return Vector of FileInfo objects.
    std::vector<FileInfo> listFiles();

    /// Download a file by ID. Returns raw bytes.
    /// @param fileId File ID from listFiles().
    /// @return Vector of bytes (empty on failure).
    std::vector<unsigned char> downloadFile(const std::string& fileId);

    // ─── Logging & Analytics ────────────────────────────────────────────────
    /// Send an activity log message to the dashboard.
    /// @param message Log message (max 500 chars).
    /// @return true if logged successfully.
    bool log(const std::string& message);

    /// Get list of currently online users.
    /// @return OnlineData with count and user list.
    OnlineData fetchOnline();

    /// Get application statistics.
    /// @return StatsData with totalUsers, onlineUsers, totalKeys, appVersion.
    StatsData fetchStats();

    // ─── Security ───────────────────────────────────────────────────────────
    /// Check if an IP or HWID is blacklisted.
    /// @param ip   IP address to check (optional, pass empty to skip).
    /// @param hwid HWID to check (optional, pass empty to skip).
    /// @return Response with data["blacklisted"] and data["reason"].
    Response checkBlacklist(const std::string& ip = "", const std::string& hwid = "");

    /// Redeem a referral code for bonus subscription days.
    /// @param code Referral code.
    /// @return Response with success, message, and data (expiresAt, rewardDays).
    Response redeemReferral(const std::string& code);

    // ─── HWID Generation ────────────────────────────────────────────────────
    /// Generate a hardware ID from the current machine.
    /// Uses disk serial number + computer name, hashed with MD5.
    /// @return 32-character lowercase hex string.
    static std::string getHWID();

    // ─── Public State ───────────────────────────────────────────────────────
    std::string sessionToken;
    std::string username;
    int level = 0;
    std::string subscription;
    std::string expiresAt;

    // App info (populated after init())
    std::string appName;
    std::string appVersion;
    bool hwidLock = false;
    bool hashCheck = false;
    bool initialized = false;

    /// Check if user has an active session
    bool isAuthenticated() const { return !sessionToken.empty(); }

private:
    std::string m_appId;
    std::string m_apiKey;
    std::string m_apiUrl;

    /// Internal HTTP POST request to the API.
    Response request(const std::string& jsonBody);

    /// Build a JSON string from key-value pairs (simple JSON builder).
    std::string buildJson(const std::map<std::string, std::string>& fields);

    /// Parse JSON response into a Response struct.
    Response parseResponse(const std::string& body);

    /// Simple JSON value extractor.
    static std::string jsonGetString(const std::string& json, const std::string& key);
    static bool jsonGetBool(const std::string& json, const std::string& key);
    static int jsonGetInt(const std::string& json, const std::string& key);

    /// Escape a string for JSON output.
    static std::string jsonEscape(const std::string& s);

    /// Compute MD5 hash of a string.
    static std::string md5(const std::string& input);
};

} // namespace authon


// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION (define AUTHON_IMPLEMENTATION in exactly ONE .cpp file)
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef AUTHON_IMPLEMENTATION

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winhttp.h>
#include <wincrypt.h>
#include <iphlpapi.h>
#include <algorithm>
#include <cctype>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "crypt32.lib")

namespace authon {

// ─── Constructor ────────────────────────────────────────────────────────────

Authon::Authon(const std::string& appId, const std::string& apiKey, const std::string& apiUrl)
    : m_appId(appId), m_apiKey(apiKey), m_apiUrl(apiUrl) {}

// ─── HWID Generation ────────────────────────────────────────────────────────

std::string Authon::getHWID() {
    std::string raw;

    // Get disk serial number via WMI command
    char buffer[256] = {0};
    FILE* pipe = _popen("wmic diskdrive get serialnumber", "r");
    if (pipe) {
        std::string output;
        while (fgets(buffer, sizeof(buffer), pipe)) {
            output += buffer;
        }
        _pclose(pipe);

        // Parse: skip first line (header), take second line
        std::istringstream stream(output);
        std::string line;
        std::getline(stream, line); // "SerialNumber"
        std::getline(stream, line); // actual serial
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        raw = line;
    }

    // Append computer name
    char computerName[256] = {0};
    DWORD size = sizeof(computerName);
    if (GetComputerNameA(computerName, &size)) {
        raw += computerName;
    }

    // Fallback if empty
    if (raw.empty()) {
        raw = "fallback_hwid";
        char userName[256] = {0};
        DWORD usize = sizeof(userName);
        if (GetUserNameA(userName, &usize)) {
            raw += userName;
        }
    }

    return md5(raw);
}

// ─── MD5 Implementation ─────────────────────────────────────────────────────

std::string Authon::md5(const std::string& input) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[16];
    DWORD cbHash = 16;

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return "";
    }

    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "";
    }

    CryptHashData(hHash, (const BYTE*)input.c_str(), (DWORD)input.length(), 0);
    CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    char hex[33] = {0};
    for (int i = 0; i < 16; i++) {
        sprintf_s(hex + i * 2, 3, "%02x", rgbHash[i]);
    }
    return std::string(hex);
}

// ─── JSON Utilities ─────────────────────────────────────────────────────────

std::string Authon::jsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 10);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:   out += c; break;
        }
    }
    return out;
}

std::string Authon::buildJson(const std::map<std::string, std::string>& fields) {
    std::string json = "{";
    bool first = true;
    for (const auto& pair : fields) {
        if (!first) json += ",";
        json += "\"" + jsonEscape(pair.first) + "\":\"" + jsonEscape(pair.second) + "\"";
        first = false;
    }
    json += "}";
    return json;
}

std::string Authon::jsonGetString(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos + search.length());
    if (pos == std::string::npos) return "";

    // Skip whitespace
    pos++;
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;

    if (pos >= json.length()) return "";

    // Handle null
    if (json.substr(pos, 4) == "null") return "";

    // Handle string value
    if (json[pos] == '"') {
        pos++;
        std::string value;
        while (pos < json.length() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.length()) {
                pos++;
                switch (json[pos]) {
                    case '"': value += '"'; break;
                    case '\\': value += '\\'; break;
                    case 'n': value += '\n'; break;
                    case 'r': value += '\r'; break;
                    case 't': value += '\t'; break;
                    default: value += json[pos]; break;
                }
            } else {
                value += json[pos];
            }
            pos++;
        }
        return value;
    }

    // Handle number or boolean as string
    std::string value;
    while (pos < json.length() && json[pos] != ',' && json[pos] != '}' && json[pos] != ']') {
        value += json[pos];
        pos++;
    }
    // Trim
    while (!value.empty() && (value.back() == ' ' || value.back() == '\t' || value.back() == '\r' || value.back() == '\n'))
        value.pop_back();
    return value;
}

bool Authon::jsonGetBool(const std::string& json, const std::string& key) {
    std::string val = jsonGetString(json, key);
    return val == "true" || val == "1";
}

int Authon::jsonGetInt(const std::string& json, const std::string& key) {
    std::string val = jsonGetString(json, key);
    if (val.empty()) return 0;
    try { return std::stoi(val); } catch (...) { return 0; }
}

Response Authon::parseResponse(const std::string& body) {
    Response resp;
    resp.success = jsonGetBool(body, "success");
    resp.message = jsonGetString(body, "message");

    // Extract data object (basic extraction)
    size_t dataPos = body.find("\"data\"");
    if (dataPos != std::string::npos) {
        size_t colonPos = body.find(':', dataPos + 6);
        if (colonPos != std::string::npos) {
            size_t start = body.find_first_of("{[", colonPos);
            if (start != std::string::npos) {
                char open = body[start];
                char close = (open == '{') ? '}' : ']';
                int depth = 1;
                size_t end = start + 1;
                while (end < body.length() && depth > 0) {
                    if (body[end] == open) depth++;
                    else if (body[end] == close) depth--;
                    end++;
                }
                std::string dataStr = body.substr(start, end - start);

                if (open == '{') {
                    // Extract key-value pairs from data object
                    // Simple parser for flat objects
                    std::vector<std::string> keys = {
                        "name", "version", "hwidLock", "hashCheck",
                        "sessionToken", "username", "level", "subscription", "expiresAt",
                        "key", "value", "count", "blacklisted", "reason",
                        "totalUsers", "onlineUsers", "totalKeys", "appVersion",
                        "rewardDays", "message"
                    };
                    for (const auto& k : keys) {
                        std::string v = jsonGetString(dataStr, k);
                        if (!v.empty()) {
                            resp.data[k] = v;
                        }
                    }
                }
                // Array handling for list_files and fetch_online
                if (open == '[') {
                    // Parse array of objects
                    size_t objStart = dataStr.find('{');
                    while (objStart != std::string::npos) {
                        size_t objEnd = dataStr.find('}', objStart);
                        if (objEnd == std::string::npos) break;
                        std::string obj = dataStr.substr(objStart, objEnd - objStart + 1);
                        std::map<std::string, std::string> item;
                        for (const auto& k : {"id", "name", "size", "minLevel", "username", "lastActive"}) {
                            std::string v = jsonGetString(obj, k);
                            if (!v.empty()) item[std::string(k)] = v;
                        }
                        if (!item.empty()) resp.dataArray.push_back(item);
                        objStart = dataStr.find('{', objEnd);
                    }
                }
            }
        }
    }

    return resp;
}

// ─── HTTP Request (WinHTTP) ─────────────────────────────────────────────────

Response Authon::request(const std::string& jsonBody) {
    Response resp;

    // Parse URL to extract host and path
    // URL format: https://api.authon.pro/v1
    std::string host = "api.authon.pro";
    std::string path = "/v1";

    // Convert to wide strings for WinHTTP
    std::wstring wHost(host.begin(), host.end());
    std::wstring wPath(path.begin(), path.end());

    HINTERNET hSession = WinHttpOpen(
        L"Authon-CPP-SDK/1.0.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) {
        resp.message = "Failed to open WinHTTP session";
        return resp;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, wHost.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        resp.message = "Failed to connect to API server";
        return resp;
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"POST", wPath.c_str(),
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        resp.message = "Failed to create HTTP request";
        return resp;
    }

    // Set timeout (15 seconds)
    DWORD timeout = 15000;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hRequest, WINHTTP_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    WinHttpSetOption(hRequest, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    // Add Content-Type header
    WinHttpAddRequestHeaders(hRequest, L"Content-Type: application/json",
                             (DWORD)-1, WINHTTP_ADDREQ_FLAG_REPLACE);

    // Send request
    BOOL result = WinHttpSendRequest(
        hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)jsonBody.c_str(), (DWORD)jsonBody.length(),
        (DWORD)jsonBody.length(), 0);

    if (!result) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        resp.message = "Failed to send request. Check your internet or API status at https://authon.pro/status";
        return resp;
    }

    // Receive response
    result = WinHttpReceiveResponse(hRequest, NULL);
    if (!result) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        resp.message = "No response from server";
        return resp;
    }

    // Check Content-Type for binary responses
    wchar_t contentType[256] = {0};
    DWORD ctSize = sizeof(contentType);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_TYPE, WINHTTP_HEADER_NAME_BY_INDEX,
                        contentType, &ctSize, WINHTTP_NO_HEADER_INDEX);
    std::wstring wContentType(contentType);
    bool isBinary = (wContentType.find(L"octet-stream") != std::wstring::npos);

    // Read response body
    std::vector<unsigned char> responseData;
    DWORD bytesAvailable = 0;
    DWORD bytesRead = 0;

    while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
        std::vector<unsigned char> buffer(bytesAvailable);
        WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead);
        responseData.insert(responseData.end(), buffer.begin(), buffer.begin() + bytesRead);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    if (isBinary) {
        resp.success = true;
        resp.binary = responseData;
        return resp;
    }

    // Parse JSON response
    std::string responseBody(responseData.begin(), responseData.end());
    if (responseBody.empty()) {
        resp.message = "Empty response from server";
        return resp;
    }

    return parseResponse(responseBody);
}

// ─── API Methods ────────────────────────────────────────────────────────────

bool Authon::init() {
    std::map<std::string, std::string> fields = {
        {"type", "init"},
        {"appId", m_appId},
        {"apiKey", m_apiKey}
    };

    Response resp = request(buildJson(fields));
    if (resp.success) {
        appName = resp.data["name"];
        appVersion = resp.data["version"];
        hwidLock = (resp.data["hwidLock"] == "true");
        hashCheck = (resp.data["hashCheck"] == "true");
        initialized = true;
    }
    return resp.success;
}

Response Authon::login(const std::string& username, const std::string& password, const std::string& hwid) {
    std::map<std::string, std::string> fields = {
        {"type", "login"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"username", username},
        {"password", password},
        {"hwid", hwid.empty() ? getHWID() : hwid}
    };

    Response resp = request(buildJson(fields));
    if (resp.success) {
        sessionToken = resp.data["sessionToken"];
        this->username = resp.data["username"];
        level = jsonGetInt(resp.data.count("level") ? resp.data["level"] : "0", "");
        try { level = std::stoi(resp.data["level"]); } catch (...) { level = 0; }
        subscription = resp.data["subscription"];
        expiresAt = resp.data["expiresAt"];
    }
    return resp;
}

Response Authon::license(const std::string& licenseKey, const std::string& hwid) {
    std::map<std::string, std::string> fields = {
        {"type", "license"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"licenseKey", licenseKey},
        {"hwid", hwid.empty() ? getHWID() : hwid}
    };

    Response resp = request(buildJson(fields));
    if (resp.success) {
        sessionToken = resp.data["sessionToken"];
        try { level = std::stoi(resp.data["level"]); } catch (...) { level = 0; }
        subscription = resp.data["subscription"];
        expiresAt = resp.data["expiresAt"];
    }
    return resp;
}

Response Authon::registerUser(const std::string& username, const std::string& password,
                              const std::string& licenseKey, const std::string& hwid) {
    std::map<std::string, std::string> fields = {
        {"type", "register"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"username", username},
        {"password", password},
        {"licenseKey", licenseKey},
        {"hwid", hwid.empty() ? getHWID() : hwid}
    };
    return request(buildJson(fields));
}

bool Authon::check() {
    if (sessionToken.empty()) return false;
    std::map<std::string, std::string> fields = {
        {"type", "check"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    return resp.success;
}

bool Authon::logout() {
    if (sessionToken.empty()) return false;
    std::map<std::string, std::string> fields = {
        {"type", "logout"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    if (resp.success) {
        sessionToken.clear();
        username.clear();
        level = 0;
        subscription.clear();
        expiresAt.clear();
    }
    return resp.success;
}

std::string Authon::getVar(const std::string& key) {
    std::map<std::string, std::string> fields = {
        {"type", "var"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"key", key},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    return resp.success ? resp.data["value"] : "";
}

bool Authon::setVar(const std::string& key, const std::string& value) {
    std::map<std::string, std::string> fields = {
        {"type", "setvar"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"key", key},
        {"value", value},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    return resp.success;
}

std::string Authon::getUserVar(const std::string& key) {
    std::map<std::string, std::string> fields = {
        {"type", "getvar"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"key", key},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    return resp.success ? resp.data["value"] : "";
}

std::vector<FileInfo> Authon::listFiles() {
    std::map<std::string, std::string> fields = {
        {"type", "list_files"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    std::vector<FileInfo> files;
    if (resp.success) {
        for (const auto& item : resp.dataArray) {
            FileInfo f;
            auto it = item.find("id"); if (it != item.end()) f.id = it->second;
            it = item.find("name"); if (it != item.end()) f.name = it->second;
            it = item.find("size"); if (it != item.end()) f.size = it->second;
            it = item.find("minLevel"); if (it != item.end()) f.minLevel = it->second;
            files.push_back(f);
        }
    }
    return files;
}

std::vector<unsigned char> Authon::downloadFile(const std::string& fileId) {
    if (sessionToken.empty() || fileId.empty()) return {};
    std::map<std::string, std::string> fields = {
        {"type", "file"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"fileId", fileId},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    return resp.binary;
}

bool Authon::log(const std::string& message) {
    std::string msg = message.substr(0, 500);
    std::map<std::string, std::string> fields = {
        {"type", "log"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"message", msg},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    return resp.success;
}

OnlineData Authon::fetchOnline() {
    std::map<std::string, std::string> fields = {
        {"type", "fetch_online"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    OnlineData data;
    if (resp.success) {
        try { data.count = std::stoi(resp.data["count"]); } catch (...) {}
        for (const auto& item : resp.dataArray) {
            auto it = item.find("username");
            if (it != item.end()) data.users.push_back(it->second);
        }
    }
    return data;
}

StatsData Authon::fetchStats() {
    std::map<std::string, std::string> fields = {
        {"type", "fetch_stats"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"sessionToken", sessionToken}
    };
    Response resp = request(buildJson(fields));
    StatsData data;
    if (resp.success) {
        try { data.totalUsers = std::stoi(resp.data["totalUsers"]); } catch (...) {}
        try { data.onlineUsers = std::stoi(resp.data["onlineUsers"]); } catch (...) {}
        try { data.totalKeys = std::stoi(resp.data["totalKeys"]); } catch (...) {}
        data.appVersion = resp.data["appVersion"];
    }
    return data;
}

Response Authon::checkBlacklist(const std::string& ip, const std::string& hwid) {
    std::map<std::string, std::string> fields = {
        {"type", "check_blacklist"},
        {"appId", m_appId},
        {"apiKey", m_apiKey}
    };
    if (!ip.empty()) fields["ip"] = ip;
    if (!hwid.empty()) fields["hwid"] = hwid;
    return request(buildJson(fields));
}

Response Authon::redeemReferral(const std::string& code) {
    std::map<std::string, std::string> fields = {
        {"type", "redeem_referral"},
        {"appId", m_appId},
        {"apiKey", m_apiKey},
        {"code", code},
        {"sessionToken", sessionToken}
    };
    return request(buildJson(fields));
}

} // namespace authon

#endif // AUTHON_IMPLEMENTATION
#endif // AUTHON_H
