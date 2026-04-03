#include "ReportGenerator.h"
#include <fstream>
#include <iostream>


void ReportGenerator::setTokens(const std::vector<Token>& tokens) {
    this->tokens = tokens;
};

void ReportGenerator::setErrors(const std::vector<ErrorToken>& errors) {
    this->errors = errors;
};

std::string ReportGenerator::getCssStyle() {
    return R"(
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
        }
        h1 {
            color: #333;
        }
        .grid {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 10px;
            margin-bottom: 20px;
        }
        .grid-column {
            display: flex;
            flex-direction: column;
        }
        .grid-item {
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        .header {
            background-color: #f0f0f0;
            font-weight: bold;
        }
    )";
};

void ReportGenerator::generateReporte1() {
    std::ofstream reporteTEST("reporte_tokens.html");

    if (!reporteTEST.is_open()) {
        std::cout << "Error al crear el reporte." << std::endl;
        return;
    }

    reporteTEST << "<!DOCTYPE html>";
    reporteTEST << "<html><head><title>Reporte Tokens</title>";
    reporteTEST << "<style>" << getCssStyle() << "</style></head><body>";
    reporteTEST << "<h1>Reporte Tokens</h1>";

    for (const auto& token : tokens) {
        reporteTEST << "<div class='grid'>";
        reporteTEST << "<div class='grid-column'><div class='grid-item header'>Token: </div><div class='grid-item'>" << token.getTypeString() << "</div></div>";
        reporteTEST << "<div class='grid-column'><div class='grid-item header'>Lexema: </div><div class='grid-item'>" << token.lexeme << "</div></div>";
        reporteTEST << "<div class='grid-column'><div class='grid-item header'>Linea: </div><div class='grid-item'>" << token.line << "</div></div>";
        reporteTEST << "<div class='grid-column'><div class='grid-item header'>Columna: </div><div class='grid-item'>" << token.column << "</div></div>";
        reporteTEST << "</div>";
    }

    reporteTEST << "</body></html>";
    reporteTEST.close();
};

