#include "ErrorManager.h"
#include "ErrorToken.h"

void ErrorManager::agregarError(std::string lex, int l, int c,
	TipoError tipo, Gravedad grav, std::string desc) {
	errores.push_back(ErrorToken(lex, l, c, tipo, grav, desc));
}

void ErrorManager::limpiarErrores() {
	errores.clear();
}

std::vector<ErrorToken> ErrorManager::getErrores() const {
	return errores;
}




