#include "Token.h"

Token::Token(TokenTypes type, std::string lexeme, int line, int column) {
    this->type = type;
    this->lexeme = lexeme;
    this->line = line;
    this->column = column;
}

std::string Token::getTypeString() const {
    switch (type) {
        case TokenTypes::IDENTIFICADOR: return "IDENTIFICADOR";
        case TokenTypes::RESERVADA: return "RESERVADA";
        case TokenTypes::CODIGO_ID: return "CODIGO_ID";
        case TokenTypes::LIT_FECHA: return "FECHA";
        case TokenTypes::LIT_HORA: return "HORA";
        case TokenTypes::LIT_INTEGER: return "INTEGER";
        case TokenTypes::LIT_STRING: return "STRING";
        case TokenTypes::ENUM_VAL: return "ENUM_VAL";
        case TokenTypes::TIPO_SANGRE: return "TIPO_SANGRE";
        case TokenTypes::DELIMITADOR: return "DELIMITADOR";
        default: return "UNKNOWN";
    }
}