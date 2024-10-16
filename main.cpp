#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include <unordered_set>
#include <cctype>

using namespace std;

enum LexemeType {
    NUMBER,
    STRING_LITERAL,
    CHARACTER_LITERAL,
    PREPROCESSOR_DIRECTIVE,
    COMMENT,
    RESERVED_WORD,
    OPERATOR,
    PUNCTUATION,
    IDENTIFIER,
    UNKNOWN
};

string getLexemeTypeName(LexemeType type) {
    switch (type) {
        case NUMBER: return "Number";
        case STRING_LITERAL: return "String Literal";
        case CHARACTER_LITERAL: return "Character Literal";
        case PREPROCESSOR_DIRECTIVE: return "Preprocessor Directive";
        case COMMENT: return "Comment";
        case RESERVED_WORD: return "Reserved Word";
        case OPERATOR: return "Operator";
        case PUNCTUATION: return "Punctuation";
        case IDENTIFIER: return "Identifier";
        default: return "Unknown";
    }
}

const unordered_set<string> reservedWords = {
        "break", "case", "catch", "class", "const", "continue", "debugger", "default",
        "delete", "do", "else", "enum", "export", "extends", "finally", "for",
        "function", "if", "import", "in", "instanceof", "let", "new", "return",
        "super", "switch", "this", "throw", "try", "typeof", "var", "void",
        "while", "with", "yield", "await", "null", "true", "false"
};

const regex numberRegex(
        R"((0[xX][0-9a-fA-F]+)|(\d+\.\d+([eE][-+]?\d+)?)|(\d+([eE][-+]?\d+)?))");
const regex stringLiteralRegex(R"("(?:\\.|[^\\"])*"|'(?:\\.|[^\\'])*')");
const regex characterLiteralRegex(R"('(?:\\.|[^\\'])')");
const regex preprocessorDirectiveRegex(R"(#.*)");
const regex commentRegex(R"(//.*?$|/\*.*?\*/)");
const regex identifierRegex(R"([a-zA-Z_$][a-zA-Z0-9_$]*)");
const regex operatorRegex(
        R"(\+\+|--|==|!=|<=|>=|&&|\|\||\+=|-=|\*=|/=|%=|<<|>>|>>>|&=|\|=|\^=|=>|[-+*/%&|^~<>!=]=?)");
const regex punctuationRegex(R"([{}()\[\],.;:?])");

struct Lexeme {
    string value;
    LexemeType type;
};

vector<Lexeme> tokenize(const string& code) {
    vector<Lexeme> tokens;
    string::const_iterator start = code.begin();
    string::const_iterator end = code.end();
    smatch match;

    while (start != end) {
        if (isspace(*start)) {
            ++start;
            continue;
        }

        if (regex_search(start, end, match, commentRegex) && match.position() == 0) {
            tokens.push_back({match.str(), COMMENT});
        }
        else if (regex_search(start, end, match, stringLiteralRegex) && match.position() == 0) {
            tokens.push_back({match.str(), STRING_LITERAL});
        }
        else if (regex_search(start, end, match, characterLiteralRegex) && match.position() == 0) {
            tokens.push_back({match.str(), CHARACTER_LITERAL});
        }
        else if (regex_search(start, end, match, numberRegex) && match.position() == 0) {
            tokens.push_back({match.str(), NUMBER});
        }
        else if (regex_search(start, end, match, preprocessorDirectiveRegex) && match.position() == 0) {
            tokens.push_back({match.str(), PREPROCESSOR_DIRECTIVE});
        }
        else if (regex_search(start, end, match, operatorRegex) && match.position() == 0) {
            tokens.push_back({match.str(), OPERATOR});
        }
        else if (regex_search(start, end, match, punctuationRegex) && match.position() == 0) {
            tokens.push_back({match.str(), PUNCTUATION});
        }
        else if (regex_search(start, end, match, identifierRegex) && match.position() == 0) {
            string value = match.str();
            if (reservedWords.count(value)) {
                tokens.push_back({value, RESERVED_WORD});
            } else {
                tokens.push_back({value, IDENTIFIER});
            }
        }
        else {
            string unknown(1, *start);
            tokens.push_back({unknown, UNKNOWN});
            ++start;
            continue;
        }
        start += match.length();
    }
    return tokens;
}

string readCodeFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file '" << filename << "'.\n";
        exit(EXIT_FAILURE);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    string filename;
    cout << "Enter the JavaScript source file name: ";
    getline(cin, filename);

    string code = readCodeFromFile(filename);
    vector<Lexeme> tokens = tokenize(code);

    cout << "\nTokens and their types:\n";
    for (const auto& token : tokens) {
        cout << "<" << token.value << ", " << getLexemeTypeName(token.type) << ">\n";
    }

    return 0;
}
