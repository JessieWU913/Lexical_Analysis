#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include "lexical_analysis.h"

using namespace std;

int lineNumber = 1;

// 关键词
bool isKeyword(const string &s)
{
    for (const auto &kw : keywords)
        if (kw.value == s)
            return true;
    return false;
}

// 运算符
int isOperator(const string &op)
{
    for (const auto &o : operators)
        if (o.value == op)
            return o.code;
    return -1;
}

// 分隔符
bool isDelimiter(const string &c)
{
    for (const auto &d : delimiters)
    {
        if (d.value == c)
            return true;
    }
    return false;
}

// 标识符
void parseIdentifier(char c, ifstream &fin)
{
    string s = "";
    s += c;

    // 读取一整个标识符
    while (isalnum(fin.peek()) || fin.peek() == '_')
        s += fin.get();
    
    // 检查是否有非法字符
    if (fin.peek() != '#' && !isspace(fin.peek()) && !isDelimiter(string(1, fin.peek())) && 
        !isOperator(string(1, fin.peek())) && fin.peek() != '\n')
    {
        // 继续读取直到遇到合法字符
        while (fin.peek() != '#' && !isspace(fin.peek()) && !isDelimiter(string(1, fin.peek())) && 
               !isOperator(string(1, fin.peek())) && fin.peek() != '\n')
        {
            s += fin.get();
        }
        
        errorLog.push_back(to_string(lineNumber) + ":ERROR:非法标识符 '" + s + "'");
        tokenStream.push_back({ERROR, s, (int)errorLog.size(), lineNumber});
        return;
    }

    // 首字符非字母，报错
    if (!isalpha(c) && c != '_')
    {
        errorLog.push_back(to_string(lineNumber) + ":ERROR:非法标识符起始字符 '" + s + "'");
        tokenStream.push_back({ERROR, s, (int)errorLog.size(), lineNumber});
        return;
    }

    // 是标识符
    if (isKeyword(s))
    {
        int code = 0;
        for (const auto &kw : keywords)
        {
            if (kw.value == s)
            {
                code = kw.code;
                break;
            }
        }
        tokenStream.push_back({KEYWORD, s, code, lineNumber});
    }

    // 不是，则加入标识符表
    else
    {
        idTable.push_back(s);
        tokenStream.push_back({IDENTIFIER, s, (int)idTable.size(), lineNumber});
    }
}

// 常数
void parseNumber(char c, ifstream &fin)
{
    string num = "";
    num += c;
    
    // 处理负数
    bool isNegative = false;
    if (c == '-')
    {
        isNegative = true;
        if (!isdigit(fin.peek()))
        {
            // 如果不是数字，可能是减法运算符
            tokenStream.push_back({OPERATOR, "-", 2, lineNumber});
            return;
        }
        num = ""; // 清空，重新开始
    }

    // 检查是否为0x或0X开头的十六进制
    if (c == '0' && (fin.peek() == 'x' || fin.peek() == 'X'))
    {
        num += fin.get(); // 读取x或X
        while (isxdigit(fin.peek()))
            num += fin.get();
        errorLog.push_back(to_string(lineNumber) + ":ERROR:不支持十六进制常量 '" + num + "'");
        tokenStream.push_back({ERROR, num, (int)errorLog.size(), lineNumber});
        return;
    }

    bool isFloat = false;
    bool hasError = false;

    while (isdigit(fin.peek()) || fin.peek() == '.' || isalpha(fin.peek()))
    {
        char next = fin.get();

        if (next == '.')
        {
            if (isFloat)
            { // 多个小数点
                errorLog.push_back(to_string(lineNumber) + ":ERROR:无效浮点数格式");
                tokenStream.push_back({ERROR, num, (int)errorLog.size(), lineNumber});
                return;
            }
            isFloat = true;
        }
        else if (isalpha(next))
        {
            // 数字中包含字母，如12a.34
            hasError = true;
            num += next;
            // 继续读取直到遇到非字母数字字符
            while (isalnum(fin.peek()) || fin.peek() == '.')
            {
                num += fin.get();
            }
            break;
        }
        num += next;
    }

    // 浮点数校验
    if (isFloat && (num.back() == '.' || num.find('.') == string::npos))
    {
        errorLog.push_back(to_string(lineNumber) + ":ERROR:无效浮点数格式，缺少小数部分");
        tokenStream.push_back({ERROR, num, (int)errorLog.size(), lineNumber});
    }
    else if (hasError)
    {
        errorLog.push_back(to_string(lineNumber) + ":ERROR:无效数字格式，包含非法字符 '" + num + "'");
        tokenStream.push_back({ERROR, num, (int)errorLog.size(), lineNumber});
    }
    else
    {
        if (isNegative)
        {
            num = "-" + num;
        }
        constTable.push_back(num);
        tokenStream.push_back({CONSTANT, num, (int)constTable.size(), lineNumber});
    }
}

// 字符串常量
void parseString(ifstream &fin)
{
    string str;

    // 读到后一个引号之前
    while (fin.peek() != '"' && fin.peek() != '#')
    {
        char c = fin.get();
        if (c == '\n')
            lineNumber++; // 处理跨行字符串
        str += c;
    }

    if (fin.peek() == '#')
    {
        errorLog.push_back(to_string(lineNumber) + ":ERROR:未闭合的字符串");
        tokenStream.push_back({ERROR, str, (int)errorLog.size(), lineNumber});
    }
    else
    {
        fin.get(); // 闭合引号
        constTable.push_back(str);
        tokenStream.push_back({CONSTANT, str, (int)constTable.size(), lineNumber});
    }
}

void parseComment(ifstream &fin, bool isMultiLine)
{
    if (!isMultiLine)
    {
        // 处理单行注释（直到换行符或EOF）
        while (fin.peek() != '\n' && !fin.eof())
        {
            fin.get(); // 消耗注释内容
        }
        if (fin.peek() == '\n')
        {
            fin.get(); // 消耗换行符
            lineNumber++;
        }
    }
    else
    {
        // 处理多行注释（直到 */ 或EOF）
        char prev = 0;
        while (fin.get(prev))
        {
            if (prev == '*' && fin.peek() == '/')
            {
                fin.get(); // 消耗 '/'
                break;
            }
            if (prev == '\n')
            {
                lineNumber++;
            }
        }
    }
}

// 主分析函数
void lexicalAnalysis(const string &filename)
{
    ifstream fin(filename);
    char c;

    while (fin.get(c))
    {
        if (c == '#')
            break;

        if (c == '\n')
            lineNumber++;
        if (isspace(c))
            continue;

        // 标识符或关键字
        if (isalpha(c) || c == '_')
        {
            parseIdentifier(c, fin);
        }
        // 数字或负数
        else if (isdigit(c) || c == '-' || (c == '.' && isdigit(fin.peek())))
        {
            parseNumber(c, fin);
        }
        // 字符串
        else if (c == '"')
        {
            parseString(fin);
        }
        // 注释处理
        else if (c == '/')
        {
            char next = fin.peek();
            if (next == '/')
            {
                fin.get();                // 消耗第二个 '/'
                parseComment(fin, false); // 处理单行注释
                continue;                 // 重要！跳过后续处理
            }
            else if (next == '*')
            {
                fin.get();               // 消耗 '*'
                parseComment(fin, true); // 处理多行注释
                continue;                // 重要！跳过后续处理
            }
            else
            {
                // 处理为除法运算符
                tokenStream.push_back({OPERATOR, "/", 4, lineNumber});
            }
        }
        // 分隔符处理
        else if (isDelimiter(string(1, c)))
        {
            for (const auto &d : delimiters)
            {
                if (d.value == string(1, c))
                {
                    tokenStream.push_back({DELIMITER, string(1, c), d.code, lineNumber});
                    break;
                }
            }
        }

        // 操作符处理
        else
        {
            // 尝试匹配双字符操作符
            string op(1, c);
            if (fin.peek() != '#')
            {
                char next = fin.peek();
                int code = isOperator(op + next);
                if (code != -1)
                {
                    op += fin.get();
                    tokenStream.push_back({OPERATOR, op, code, lineNumber});
                    continue;
                }
            }

            // 单字符操作符或错误
            int code = isOperator(op);
            if (code != -1)
            {
                tokenStream.push_back({OPERATOR, op, code, lineNumber});
            }
            else
            {
                // 处理未定义的字符
                string errorMsg = "未定义的字符: '" + string(1, c) + "'";
                errorLog.push_back(to_string(lineNumber) + ": " + errorMsg);
                tokenStream.push_back({ERROR, string(1, c), (int)errorLog.size(), lineNumber});
            }
        }
    }

    fin.close();
}

// 输出主程序串
void outputResults()
{
    // 输出标识符表
    ofstream idTableFile("identifier_table.txt");
    for (size_t i = 0; i < idTable.size(); i++) {
        idTableFile << i + 1 << ": " << idTable[i] << endl;
    }
    idTableFile.close();

    // 输出常数表
    ofstream constTableFile("constant_table.txt");
    for (size_t i = 0; i < constTable.size(); i++) {
        constTableFile << i + 1 << ": " << constTable[i] << endl;
    }
    constTableFile.close();

    // 输出单词串
    ofstream tokenStreamFile("token_stream.txt");
    for (const auto &t : tokenStream) {
        switch (t.type) {
        case KEYWORD:
            tokenStreamFile << "KEYWORD: " << t.code << " at line: " << t.line << endl;
            break;
        case IDENTIFIER:
            tokenStreamFile << "IDENTIFIER: " << t.code << " at line: " << t.line << endl;
            break;
        case CONSTANT:
            tokenStreamFile << "CONSTANT: " << t.code << " at line: " << t.line << endl;
            break;
        case OPERATOR:
            tokenStreamFile << "OPERATOR: " << t.code << " at line: " << t.line << endl;
            break;
        case DELIMITER:
            tokenStreamFile << "DELIMITER: " << t.code << " at line: " << t.line << endl;
            break;
        case ERROR:
            tokenStreamFile << "ERROR: " << t.code << " at line: " << t.line << endl;
            break;
        }
    }
    tokenStreamFile.close();

    // 输出错误日志
    ofstream errorLogFile("error_log.txt");
    for (const auto &err : errorLog) {
        errorLogFile << err << endl;
    }
    errorLogFile.close();
}

int main()
{
    lexicalAnalysis("source.txt");
    outputResults();
    return 0;
}