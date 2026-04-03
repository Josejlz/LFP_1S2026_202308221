#pragma once
#include <wx/log.h>
#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <wx/string.h>
#include "Token.h"
#include "ErrorManager.h"
#include <vector>

using namespace std;
class ErrorManager;

    class LexicalAnalyzer{
    private:
        string input;
        int pos;
        int line;
        int col;

        char peek();
        char advance();

    public:
        LexicalAnalyzer();
		void setText(std::string t); //Para establecer el texto a analizar
        void analyze();
        void nextToken(wxString filepath);
        vector<Token> tokens;
		ErrorManager* errorManager;


};

