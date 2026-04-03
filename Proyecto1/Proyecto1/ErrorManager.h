#pragma once
#include <string>
#include <vector>
#include "ErrorToken.h"


class ErrorManager
{
public:
	std::vector<ErrorToken> errores;
	void limpiarErrores();
	std::vector<ErrorToken> getErrores() const;
	
	void agregarError(std::string lex, int l, int c, TipoError tipo, Gravedad grav, std::string desc);

};

