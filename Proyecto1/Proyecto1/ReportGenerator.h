#pragma once
#include <string>
#include <vector>
#include "Token.h"
#include "ErrorToken.h"
class ReportGenerator
{
    std::vector<Token> tokens;
    std::vector<ErrorToken> errors;

public:
    std::string getCssStyle();
    void generateReporte1();
    void generateReporte2();
    void generateReporte3();
    void generateReporte4();
    void generateGraphviz();
    void setTokens(const std::vector<Token>& tokens);
    void setErrors(const std::vector<ErrorToken>& errors);
};

