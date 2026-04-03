#include "LexicalAnalyzer.h"
#include <wx/log.h>
#include <wx/string.h>
#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <string>


LexicalAnalyzer::LexicalAnalyzer() {
	pos = 0;
	line = 1;
	col = 1;
	input = "";
	errorManager = new ErrorManager();
};

void LexicalAnalyzer::setText(std::string t) {
	input = t;
};

static TokenTypes clasificacionPalabra(const std::string& p) {
	if (p == "HOSPITAL" || p == "PACIENTES" || p == "MEDICOS" || p == "CITAS" || p == "DIAGNOSTICOS") {
		return TokenTypes::RESERVADA;
	}
	if (p == "CARDIOLOGIA" || p == "NEUROLOGIA" || p == "PEDIATRIA" || p == "CIRUGIA" || p == "MEDICINA_GENERAL" || p == "ONCOLOGIA" || p == "PSICOLOGIA" || p == "DIARIA" || p == "CADA_8_HORAS" || p == "CADA_12_HORAS" || p == "SEMANAL") {
		return TokenTypes::ENUM_VAL;
	}

	return TokenTypes::IDENTIFICADOR;

};

static TokenTypes clasificarContenidoCadena(const std::string& content) {
    //tipos de sangre en un enum pero de strings
    static const std::vector<std::string> tiposSangre = {
        "A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"
    };


    for (const auto& s : tiposSangre)
        if (content == s) return TokenTypes::TIPO_SANGRE;

    // Verifica si es codigo con el formato
    if (content.size() >= 5 &&
        std::isupper((unsigned char)content[0]) &&
        std::isupper((unsigned char)content[1]) &&
        std::isupper((unsigned char)content[2]) &&
        content[3] == '-' &&
        std::all_of(content.begin() + 4, content.end(), ::isdigit))
        return TokenTypes::CODIGO_ID;

    // devuelve como cadena regular
    return TokenTypes::LIT_STRING;
}



char LexicalAnalyzer::peek() {
	if (pos < input.size()) {
		return input[pos];
	}
	return '\0'; // eof
};

char LexicalAnalyzer::advance() {
    if (pos < input.size()) {
        char currentChar = input[pos++];

        if (currentChar == '\n') {
            line++;
            col = 1;
        } 
        else {
            if ((static_cast<unsigned char>(currentChar) & 0xC0) != 0x80) {
                col++;
            }
        }
        return currentChar;
    }
    return '\0'; // eof
}


void LexicalAnalyzer::nextToken(wxString filepath) {
    std::string filename = filepath.ToStdString();

    if (filename.find(".med") == std::string::npos) {
        wxLogMessage("El archivo no es valido, o no se selecciono uno.");
        return;
    }

    std::ifstream archivo(filename);
    if (!archivo.is_open()) {
        errorManager->agregarError("", line, col, TipoError::UNRECOGNIZED_SYMBOL,
            Gravedad::LIT_CRITICO, "No se pudo abrir el archivo.");
        return;
    }

    std::stringstream buffer;
    buffer << archivo.rdbuf();
    input = buffer.str();
    archivo.close();

    pos   = 0;
    line  = 1;
    col   = 1;
    tokens.clear();

    int state = 0;
    std::string currentLexeme = "";
    int startLine = 1, startCol = 1; // posicion de inicio del token actual

    while (true) {
        char c = peek();

        switch (state) {

        // inicio
        case 0:
            startLine = line;
            startCol  = col;

            if (c == '\n') {
                line++;
                col = 1;
                advance();
                break;
            }
            if (std::isspace(static_cast<unsigned char>(c))) {
                advance();
                break;
            }
            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                currentLexeme += advance();
                state = 1;
                break;
            }
            if (std::isdigit(static_cast<unsigned char>(c))) {
                currentLexeme += advance();
                state = 2;
                break;
            }
            if (c == '"') {
                advance(); //consumo de comilla
                state = 3;
                break;
            }
            if (c == ':' || c == '[' || c == ']' ||
                c == '{' || c == '}' || c == ',' || c == ';') {
                currentLexeme += advance();
                tokens.push_back(Token(TokenTypes::DELIMITADOR, currentLexeme, startLine, startCol));
                currentLexeme = "";
                break;
            }
            if (c == '\0') {
                tokens.push_back(Token(TokenTypes::TOKEN_EOF, "EOF", line, col));
                return;
            }
            if (static_cast<unsigned char>(c) > 127) {
                currentLexeme += advance();
                state = 1;
                break;
            }
            errorManager->agregarError(
                std::string(1, c), startLine, startCol,
                TipoError::UNRECOGNIZED_SYMBOL, Gravedad::LIT_ERROR,
                "Símbolo no reconocido: '" + std::string(1, c) + "'"
            );
            advance();
            break;

        // Para palabras reservadas
        case 1:
            if (std::isalnum(static_cast<unsigned char>(c)) ||
                c == '_' ||
                static_cast<unsigned char>(c) > 127) {
                currentLexeme += advance();
            }
            else if (c == '-') {
                currentLexeme += advance();
                state = 20;
            }
            else {
                TokenTypes tipo = clasificacionPalabra(currentLexeme);
                tokens.push_back(Token(tipo, currentLexeme, startLine, startCol));
                currentLexeme = "";
                state = 0;
            }
            break;

        // para ver si es fecha u hora
        case 2:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                currentLexeme += advance();
            }
            else if (c == '-' && currentLexeme.size() == 4) {
                currentLexeme += advance();
                state = 10;
            }
            else if (c == ':' && currentLexeme.size() == 2) {
                currentLexeme += advance();
                state = 15;
            }
            else {
                // Es un entero normal
                tokens.push_back(Token(TokenTypes::LIT_INTEGER, currentLexeme, startLine, startCol));
                currentLexeme = "";
                state = 0;
            }
            break;

        // =====================================================================
        // ESTADO 3 — dentro de comillas dobles
        // Puede ser: LIT_STRING, TIPO_SANGRE o CODIGO_ID
        // =====================================================================
        case 3:
            if (c == '"') {
                advance(); // consume comilla de cierre
                // Clasificar según el contenido acumulado
                TokenTypes tipo = clasificarContenidoCadena(currentLexeme);
                // Reconstruir el lexema con comillas para mostrarlo en la tabla
                tokens.push_back(Token(tipo, "\"" + currentLexeme + "\"", startLine, startCol));
                currentLexeme = "";
                state = 0;
            }
            else if (c == '\n' || c == '\0') {
                // Cadena sin cerrar, critico
                errorManager->agregarError(
                    "\"" + currentLexeme, startLine, startCol,
                    TipoError::UNCLOSED_STRING, Gravedad::LIT_CRITICO,
                    "Cadena sin cerrar antes del fin de línea"
                );
                currentLexeme = "";
                state = 0;
                if (c == '\n') { line++; col = 1; advance(); }
            }
            else {
                currentLexeme += advance();
            }
            break;

        //para el mes de la fecha
        case 10:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                currentLexeme += advance();
            }
            else if (c == '-' && currentLexeme.size() == 7) {
                
                currentLexeme += advance();
                state = 11;
            }
            else {
                errorManager->agregarError(
                    currentLexeme, startLine, startCol,
                    TipoError::INVALID_DATE, Gravedad::LIT_ERROR,
                    "Fecha malformada. Formato esperado: AAAA-MM-DD"
                );
                currentLexeme = "";
                state = 0;
            }
            break;

        //dia de la fecha
        case 11:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                currentLexeme += advance();
            }
            else {
                if (currentLexeme.size() == 10) {
                    // Validar rangos
                    int mes = std::stoi(currentLexeme.substr(5, 2));
                    int dia = std::stoi(currentLexeme.substr(8, 2));

                    if (mes < 1 || mes > 12) {
                        errorManager->agregarError(
                            currentLexeme, startLine, startCol,
                            TipoError::INVALID_DATE, Gravedad::LIT_ERROR,
                            "Mes '" + std::to_string(mes) + "' fuera de rango (01-12)"
                        );
                    }
                    else if (dia < 1 || dia > 31) {
                        errorManager->agregarError(
                            currentLexeme, startLine, startCol,
                            TipoError::INVALID_DATE, Gravedad::LIT_ERROR,
                            "Día '" + std::to_string(dia) + "' fuera de rango (01-31)"
                        );
                    }
                    else {
                        tokens.push_back(Token(TokenTypes::LIT_FECHA, currentLexeme, startLine, startCol));
                    }
                }
                else {
                    errorManager->agregarError(
                        currentLexeme, startLine, startCol,
                        TipoError::INVALID_DATE, Gravedad::LIT_ERROR,
                        "Fecha malformada. Formato esperado: AAAA-MM-DD"
                    );
                }
                currentLexeme = "";
                state = 0;
            }
            break;

        //minutos de la hora
        case 15:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                currentLexeme += advance();
            }
            else {
                if (currentLexeme.size() == 5) {
                    int hh = std::stoi(currentLexeme.substr(0, 2));
                    int mm = std::stoi(currentLexeme.substr(3, 2));

                    if (hh > 23 || mm > 59) {
                        errorManager->agregarError(
                            currentLexeme, startLine, startCol,
                            TipoError::INVALID_TIME, Gravedad::LIT_ERROR,
                            "Hora '" + currentLexeme + "' fuera de rango (00:00-23:59)"
                        );
                    }
                    else {
                        tokens.push_back(Token(TokenTypes::LIT_HORA, currentLexeme, startLine, startCol));
                    }
                }
                else {
                    errorManager->agregarError(
                        currentLexeme, startLine, startCol,
                        TipoError::INVALID_TIME, Gravedad::LIT_ERROR,
                        "Hora malformada. Formato esperado: HH:MM"
                    );
                }
                currentLexeme = "";
                state = 0;
            }
            break;

        //para IDs
        case 20:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                currentLexeme += advance();
            }
            else {
                // Validar formato: exactamente 3 letras + '-' + al menos 1 dígito
                bool formatoValido = currentLexeme.size() >= 5 &&
                    std::isupper((unsigned char)currentLexeme[0]) &&
                    std::isupper((unsigned char)currentLexeme[1]) &&
                    std::isupper((unsigned char)currentLexeme[2]) &&
                    currentLexeme[3] == '-';

                if (formatoValido) {
                    tokens.push_back(Token(TokenTypes::CODIGO_ID, currentLexeme, startLine, startCol));
                }
                else {
                    errorManager->agregarError(
                        currentLexeme, startLine, startCol,
                        TipoError::INVALID_ID_CODE, Gravedad::LIT_ERROR,
                        "Código ID malformado. Formato esperado: XXX-000"
                    );
                }
                currentLexeme = "";
                state = 0;
            }
            break;
        }
    }
};







