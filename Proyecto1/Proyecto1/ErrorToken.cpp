#include "ErrorToken.h"
#include <string>
#include <vector>

ErrorToken::ErrorToken(std::string lex, int l, int c, TipoError tipo, Gravedad grav, std::string desc) {
	lexeme = lex;
	line = l;
	column = c;
	tipoError = tipo;
	gravedad = grav;
	description = desc;
};

std::string ErrorToken::getStringTipoError() const {
	switch (tipoError) {
	case TipoError::UNRECOGNIZED_SYMBOL: return "Símbolo no reconocido";
	case TipoError::UNCLOSED_STRING:  return "Cadena sin cerrar";
	case TipoError::INVALID_NUMBER:  return "Número inválido";
	case TipoError::INVALID_DATE: return "Fecha inválida";
	case TipoError::INVALID_TIME: return "Hora fuera de rango";
	case TipoError::INVALID_BLOOD_TYPE:  return "Tipo de sangre inválido";
	case TipoError::INVALID_SPECIALTY: return "Especialidad no reconocida";
	case TipoError::INVALID_ID_CODE:  return "Código ID malformado";
	default: return "Error desconocido";
	}
}

std::string ErrorToken::getStringGravedad() const {
	switch (gravedad) {
	case Gravedad::LIT_ERROR: return "ERROR";
	case Gravedad::LIT_CRITICO: return "CRITICO";
	default: return "UNKNOWN";
	}
}


