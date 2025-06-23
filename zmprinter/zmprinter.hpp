#pragma once

#include <array>
#include <chrono>
#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace zm {

// Forward declaration
class printer;

// ==== Token Interface ====
struct templateToken {
    virtual void render(std::ostream& out, const printer& p, const char* dynamicStr) const = 0;
    virtual ~templateToken() = default;
};

// ==== Token Types ====
struct literalToken : templateToken {
    std::string text;
    explicit literalToken(std::string txt) : text(std::move(txt)) {}
    void render(std::ostream& out, const printer&, const char*) const override;
};

struct timeToken : templateToken {
    static const std::chrono::time_point<std::chrono::steady_clock> startTime;
    void render(std::ostream& out, const printer&, const char*) const override;
};

struct messageToken : templateToken {
    void render(std::ostream& out, const printer& p, const char* dynamicStr) const override;
};

// ==== Printer Class ====
class printer {
public:
    explicit printer(const char* config) : tmpl(config) {
        parseTemplate(tmpl);
    }

    template<typename... Args>
    void print(const char* dynamicStr, const Args&... args) {
        collect(args...);
        render(dynamicStr);
    }

    std::string parsePlaceholders(const char* str) const;

private:
    std::string tmpl;
    std::stringstream output;
    std::stringstream converter;
    std::array<std::string, 9> argBuffer;
    int argIndex = 0;
    std::vector<std::unique_ptr<templateToken>> tokens;

    void parseTemplate(const std::string& source);

    void render(const char* dynamicStr);

    template<typename Head, typename... Tail>
    void collect(const Head& head, const Tail&... tail) {
        converter.str("");
        converter.clear();
        converter << head;
        if (argIndex < argBuffer.size()) {
            argBuffer[argIndex++] = converter.str();
        }
        collect(tail...);
    }

    void collect() {}

    static bool matchKey(const char* str, unsigned i, char& key, unsigned& next) {
        constexpr unsigned keySize = 4;
        next = i + keySize;
        if (str[i] != '{' || str[i + 1] != '%' || str[i + 3] != '}') return false;
        key = str[i + 2];
        return true;
    }
};


// ==== Static Time Initialization ====
#ifdef ZMPRINTER_IMPL
const std::chrono::time_point<std::chrono::steady_clock> timeToken::startTime =
    std::chrono::steady_clock::now();

// ==== Token Implementations ====
void literalToken::render(std::ostream& out, const printer&, const char*) const {
  out << text;
}
void timeToken::render(std::ostream& out, const printer&, const char*) const {
    auto elapsed = std::chrono::steady_clock::now() - startTime;
    int secs = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    int h = secs / 3600, m = (secs % 3600) / 60, s = secs % 60;
    out << h << ':' << m << ':' << s;
}

void messageToken::render(std::ostream& out, const printer& p, const char* dynamicStr) const {
    out << p.parsePlaceholders(dynamicStr);
}

// ==== Template Parsing ====
void printer::parseTemplate(const std::string& source) {
    unsigned i = 0, segmentStart = 0, next = 0;
    char key;

    while (i < source.size()) {
        if (!matchKey(source.c_str(), i, key, next)) {
            ++i;
            continue;
        }

        if (i > segmentStart) {
            tokens.emplace_back(std::make_unique<literalToken>(source.substr(segmentStart, i - segmentStart)));
        }

        switch (key) {
            case 't': tokens.emplace_back(std::make_unique<timeToken>()); break;
            case 'm': tokens.emplace_back(std::make_unique<messageToken>()); break;
        }

        i = next;
        segmentStart = next;
    }

    if (segmentStart < source.size()) {
        tokens.emplace_back(std::make_unique<literalToken>(source.substr(segmentStart)));
    }
}
//======rendering=========
void printer::render(const char* dynamicStr) {
  for (const auto& token : tokens) {
    token->render(output, *this, dynamicStr);
  }
  std::cout << output.str();
  output.str("");
  output.clear();
	argIndex = 0;
}
//======parsingPlaceholders=========
std::string printer::parsePlaceholders(const char* str) const {
  std::stringstream result;
  unsigned i = 0, segmentStart = 0, next = 0;
  char key;

  while (true) {
    if (str[i] == '\0') {
      result << std::string_view(&str[segmentStart], i - segmentStart);
      break;
    }

    if (!matchKey(str, i, key, next)) {
      ++i;
      continue;
    }

    if (std::isdigit(key)) {
      result << std::string_view(&str[segmentStart], i - segmentStart);
      result << argBuffer[key - '0'];
      segmentStart = next;
      i = next;
    } else {
      ++i;
    }
  }

  return result.str();
}

#endif

} // namespace zm

