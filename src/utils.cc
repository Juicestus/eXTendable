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

std::string readFile(const std::string& path) {
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) return ""; // std::printf("Could not open file\n");
    return std::string(std::istreambuf_iterator<char>(inputFile),
                       std::istreambuf_iterator<char>());
}

std::string ignoreShebang(const std::string& code) {
    return (code.substr(0, 2) == "#!") ? code.substr(code.find_first_of('\n')) : code;
}

std::string exeFilename() {
    char buffer[1024];
#ifdef _WIN32
    GetModuleFileName(NULL, buffer, 1024);
#else
    getcwd(buffer, 256);
#endif
    return std::string(buffer);
}

std::string UtilPath::getPath() {
    return path;
}

std::string UtilPath::getFilename() {
    return path.substr(path.find_last_of("/\\") + 1);
}

std::string UtilPath::getExtension() {
    return getFilename().substr(getFilename().find_last_of(".") + 1);
}

std::string UtilPath::getBasename() {
    return getFilename().substr(0, getFilename().find_last_of("."));
}

std::string UtilPath::getDirname() {
    std::string fn = path.substr(0, path.find_last_of("/\\"));
    return fn == getFilename() ? "." : fn;
}

std::string UtilPath::printable() {
    return path + "\nDir -> " + getDirname() + "\nFile -> " + getFilename() +
           "\nExt -> " + getExtension() + "\nBase -> " + getBasename() + "\n";
}

std::vector<std::string> splitStringIntoVector
        (const std::string& subject, const char delimiter) {
    std::vector<std::string> tokens;
    std::string currentToken = "";
    for (int i = 0; i < subject.length(); i++) {
        if (subject[i] == delimiter) {
            if (currentToken != "") {
                tokens.push_back(currentToken);
            }
            currentToken = "";
        } else {
            currentToken += subject[i];
        }
    }
    tokens.push_back(currentToken);
    return tokens;
}

std::string replaceAllStringInString(std::string subject,
        const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

std::string stripWhitespace(const std::string& subject) {
    std::string result = "";
    for (const char c : subject)
        if (!isWhitespace(c))
            result += c;
    return result;
}