#ifndef LEXICAL_ANALYSIS_H
#define LEXICAL_ANALYSIS_H

#include <vector>
#include <string>

using namespace std;

// Token类
enum TokenType
{
    KEYWORD = 1, // 1
    IDENTIFIER,  // 2
    CONSTANT,    // 3
    OPERATOR,    // 4
    DELIMITER,   // 5
    ERROR        // 6
};

// 表结构
struct Symbol
{
    string value;
    int code; // 在表中的编号
};

// 词法单元
struct Token
{
    TokenType type;
    string value;
    int code; // 在表中的索引
    int line; // 行号
};

// 符号表
vector<Symbol> keywords =
    {
        {"if", 1},
        {"else", 2},
        {"while", 3},
        {"const", 4},
        {"return", 5},
        {"int", 6},
        {"float", 7},
        {"main", 8}};

vector<Symbol> operators =
    {
        {"+", 1},
        {"-", 2},
        {"*", 3},
        {"/", 4},
        {"=", 5},
        {"==", 6},
        {">", 7},
        {"<", 8},
        {">=", 9},
        {"<=", 10},
        {"!=", 11},
        {"&&", 12},
        {"||", 13},
        {"!", 14},
        {"+=", 15},
        {"-=", 16},
        {"*=", 17},
        {"/=", 18},
        {"->", 19},
        {"%", 20},
        {"++", 21},
        {"--", 22},
        {"&", 23},
        {"|", 24}};

vector<Symbol> delimiters =
    {
        {";", 1},
        {",", 2},
        {"(", 3},
        {")", 4},
        {"{", 5},
        {"}", 6},
        {"[", 7},
        {"]", 8}};

// 动态表
vector<string> idTable;
vector<string> constTable;
vector<string> errorLog;

// token表
vector<Token> tokenStream;

#endif


