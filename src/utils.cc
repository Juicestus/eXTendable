#include "utils.h"

bool isWhitespace(char ch) {
    return (ch == ' ') || (ch == '\t') || (ch == '\n') || (ch == '\r');
}

bool isNumeric(char ch) {
    return (ch >= '0') && (ch <= '9');
}

bool isNumber(const std::string& str) {
    for (int i = 0; i < str.size(); i++)
        if (!isNumeric(str[i])) return false;
    return true;
}
bool isHexadecimal(char ch) {
    return ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) ||
           ((ch >= 'A') && (ch <= 'F'));
}

bool isAlpha(char ch) {
    return ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) ||
           ch == '_';
}

bool isIdentString(const char* s) {
    if (!isAlpha(*s)) return false;

    while (*s) {
        if (!(isAlpha(*s) || isNumeric(*s))) return false;
        s++;
    }

    return true;
}

void replace(std::string& str, char textFrom, const char* textTo) {
    int sLen = strlen(textTo);
    size_t p = str.find(textFrom);
    while (p != std::string::npos) {
        str = str.substr(0, p) + textTo + str.substr(p + 1);
        p = str.find(textFrom, p + sLen);
    }
}

std::string getXTString(const std::string& str) {
    std::string nStr = str;
    for (size_t i = 0; i < nStr.size(); i++) {
        const char* replaceWith = "";
        bool replace = true;

        switch (nStr[i]) {
        case '\\': replaceWith = "\\\\"; break;
        case '\n': replaceWith = "\\n"; break;
        case '\r': replaceWith = "\\r"; break;
        case '\a': replaceWith = "\\a"; break;
        case '"': replaceWith = "\\\""; break;
        default: {
            int nCh = ((int)nStr[i]) & 0xFF;
            if (nCh < 32 || nCh > 127) {
                char buffer[5];
                std::snprintf(buffer, 5, "\\x%02X", nCh);
                replaceWith = buffer;
            } else
                replace = false;
        }
        }

        if (replace) {
            nStr = nStr.substr(0, i) + replaceWith + nStr.substr(i + 1);
            i += strlen(replaceWith) - 1;
        }
    }
    return "\"" + nStr + "\"";
}

bool isAlphaNum(const std::string& str) {
    if (str.size() == 0) return true;
    if (!isAlpha(str[0])) return false;
    for (int i = 0; i < str.size(); i++)
        if (!(isAlpha(str[i]) || isNumeric(str[i]))) return false;
    return true;
}

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename... Args>
std::string stringFormat(const std::string& format, Args... args) {
    // int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    int size_s = _snprintf(nullptr, 0, format.c_str(), args...) + 1;
    if (size_s <= 0) throw std::runtime_error("Error during formatting.");
    auto size = static_cast<size_t>(size_s);
    auto buf = make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}