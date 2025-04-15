#ifndef LEXICAL_ANALYSIS_H
#define LEXICAL_ANALYSIS_H

using namespace std;

// 基础Token类
enum TokenType
{
    KEYWORD,
    IDENTIFIER,
    CONSTANT,
    OPERATOR,
    DELIMITER
};

// 关键字类型
enum KeywordType
{
    IF,
    ELSE
};

// 常量类型
enum ConstantType
{
    INT,
    FLOAT,
    INVALID
};

// 运算符类型
enum OperatorType
{
    PLUS,   // +
    MINUS,  // -
    MUL,    // *
    DIV,    // /
    ASSIGN, // =
    EQ,     // ==
    NE,     // !=
    LT,     // <
    LE,     // <=
    GT,     // >
    GE      // >=
};

// 分隔符类型
enum DelimiterType
{
    SEMICOLON, // ;
    PAREN_L,   // (
    PAREN_R    // )
};

struct Token
{
    TokenType type;
    string value;
};

// 符号表条目
struct SymbolEntry
{
    std::string name;
};

struct ConstantEntry
{
    std::string value;
    ConstantType type;
};
struct TokenEntry
{
    TokenType main_type;
    int subtype;     // 用于OPERATOR/KEYWORD子类型
    int table_index; // 符号表/常量表索引
};

// 全局词法分析数据
vector<SymbolEntry> symbol_table;
vector<ConstantEntry> constant_table;
vector<TokenEntry> token_stream;
vector<string> error_log;

void output_token(Token token);

#endif