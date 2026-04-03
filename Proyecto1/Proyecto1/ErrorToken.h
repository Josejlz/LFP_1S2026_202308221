#pragma once
#include <string>
#include <vector>

enum class TipoError {
	UNRECOGNIZED_SYMBOL,
	UNCLOSED_STRING,
	INVALID_NUMBER,
	INVALID_DATE,
	INVALID_TIME,
	INVALID_BLOOD_TYPE,    // "O++" por ejemplo
	INVALID_SPECIALTY,     // especialidad no reconocida
	INVALID_ID_CODE
};

enum class Gravedad {
	LIT_ERROR, //continua normal
	LIT_CRITICO //No continua
};

class ErrorToken
{
public:
	std::string lexeme;
	int line;
	int column;
	TipoError tipoError;
	Gravedad gravedad;
	std::string description;

	ErrorToken(std::string lex, int l, int c, TipoError tipo, Gravedad grav, std::string desc);

	std::string getStringGravedad() const;

	std::string getStringTipoError() const;




	
};

