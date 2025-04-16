#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <lexical_analysis.h>

using namespace std;

// 标识符判断 identifier -> letter_ (letter_ | digit)*

bool isLetter_(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' ? 1 : 0;
}

bool isDigit(char c)
{
    return c >= '0' && c <= '9' ? 1 : 0;
}

// 运算符转换到枚举值
OperatorType get_OpType(string op)
{
    // 双字符优先判断
    if (op == "==")
        return EQ;
    if (op == "!=")
        return NE;
    if (op == "<=")
        return LE;
    if (op == ">=")
        return GE;

    // 单字符
    if (op == "+")
        return PLUS;
    if (op == "-")
        return MINUS;
    if (op == "*")
        return MUL;
    if (op == "/")
        return DIV;
    if (op == "=")
        return ASSIGN;
    if (op == "<")
        return LT;
    if (op == ">")
        return GT;

    return ASSIGN;
}

// 运算符映射表
unordered_map<string, int> OPERATOR_MAP =
    {
        {"+", PLUS},
        {"-", MINUS},
        {"*", MUL},
        {"/", DIV},
        {"=", ASSIGN},
        {"==", EQ},
        {"!=", NE},
        {"<", LT},
        {"<=", LE},
        {">", GT},
        {">=", GE},
        {";", SEMICOLON},
        {"(", PAREN_L},
        {")", PAREN_R}};

// 判断是否为运算符
int isOperator(const string &op)
{
    auto it = OPERATOR_MAP.find(op);
    return (it != OPERATOR_MAP.end()) ? it->second : -1;
}

// 关键字映射表
unordered_map<string, int> KEYWORD_MAP =
    {
        {"if", IF},
        {"else", ELSE},
};

// 判断是否为关键字
int isKeyword(const string &ident)
{
    auto it = KEYWORD_MAP.find(ident);
    return (it != KEYWORD_MAP.end()) ? it->second : -1;
}

void lexer(const string &source)
{
    // 标记当前扫描字符位置
    size_t p = 0;

    // 源文件字符串长度
    size_t size = source.size();

    // 开始扫描
    while (p < size)
    {
        // 跳过空白字符
        if (isspace(source[p]))
        {
            p++;
            continue;
        }

        // 处理标识符和关键字
        if (isLetter_(source[p]))
        {
            size_t start = p++;

            while (p < size && (isLetter_(source[p] || isDigit(source[p]))))
                p++;

            string ident = source.substr(start, p - start);

            // 判断关键字
            int keyword_type = isKeyword(ident);
            if (keyword_type != -1)
            {
                token_stream.push_back({KEYWORD, keyword_type, -1});
            }
            // 为标识符
            else
            {
                // 添加到符号表
                auto it = find_if(symbol_table.begin(), symbol_table.end(),
                                  [&](const SymbolEntry &e)
                                  { return e.name == ident; });
                if (it == symbol_table.end())
                {
                    symbol_table.push_back({ident});
                    it = symbol_table.end() - 1;
                }
                int index = distance(symbol_table.begin(), it);
                token_stream.push_back({IDENTIFIER, -1, index});
            }
            continue;
        }

        // 处理数字常量
        if (isDigit(source[p]) || source[p] == '.')
        {
            // 简化的数字解析逻辑（实际需要完整实现）
            size_t start = p;
            bool is_float = false;
            while (p < source.size())
            {
                if (isDigit(source[p]))
                {
                    ++p;
                    continue;
                }
                if (source[p] == '.' || source[p] == 'e' || source[p] == 'E')
                {
                    is_float = true;
                    ++p;
                }
                else
                    break;
            }

            // 添加到常量表
            string value = source.substr(start, p - start);
            ConstantType type = is_float ? FLOAT : INT; // 简化类型判断
            constant_table.push_back({value, type});
            token_stream.push_back({CONSTANT, -1, (int)constant_table.size() - 1});
            continue;
        }

        // 处理运算符
        vector<string> ops = {"==", "!=", "<=", ">=", "+", "-", "*", "/", "=", "<", ">", ";", "(", ")"};
        bool found = false;
        for (const auto &op : ops)
        {
            if (source.substr(p, op.size()) == op)
            {
                token_stream.push_back({OPERATOR, get_OpType(op), -1});
                p += op.size();
                found = true;
                break;
            }
        }
        if (found)
            continue;

        // 错误处理
        error_log.push_back("Invalid character: " + std::string(1, source[p]));
        ++p;
    }
}

void print_symbol_entry(vector<SymbolEntry> symbol_table)
{
}

void print_constant_entry(vector<ConstantEntry> constant_table)
{
}

void print_token_entry(vector<TokenEntry> token_stream)
{
}

void print_error_log(vector<string> error_log)
{
}

int main()
{
    return 0;
}