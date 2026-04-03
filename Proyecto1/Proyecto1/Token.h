#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>


enum class TokenTypes{
    IDENTIFICADOR, 
    RESERVADA,
    CODIGO_ID, 
    LIT_FECHA, 
    LIT_HORA, 
    LIT_INTEGER, 
    LIT_STRING,
    ENUM_VAL,
    TIPO_SANGRE, 
    DELIMITADOR
};


class Token {
public:
	TokenTypes type;
    std::string lexeme;   
    int line;          
    int column;        

    Token(TokenTypes type, std::string lexeme, int line, int column);

    std::string getTypeString() const;

};

#endif


