#include "MainFrame.h"
#include <wx/filedlg.h>
#include <wx/log.h>
#include <wx/wx.h>
#include <string>
#include "LexicalAnalyzer.h"

wxString filepath = "";
LexicalAnalyzer* lexicalAnalyzer;

void MainFrame::setLexicalAnalyzer(LexicalAnalyzer* la) {
	lexicalAnalyzer = la;
};

void MainFrame::setReportGenerator(ReportGenerator* rg) {
	reportGenerator = rg;
}

enum BTN_ID {
	LOAD_ID = 1,
	GENREPORTES_ID = 2,
	ANALYZE_ID = 3,
	TEXTAREA_ID = 4,
	PNGHANDLER_ID = 5
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_BUTTON(LOAD_ID, MainFrame::OnButtonLoadClicked)
EVT_BUTTON(GENREPORTES_ID, MainFrame::OnButtonGenReportesClicked)
EVT_BUTTON(ANALYZE_ID, MainFrame::OnButtonAnalyzeClicked)
wxEND_EVENT_TABLE()


MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {

	panel = new wxPanel(this);

	btnLoad = new wxButton(panel, LOAD_ID, "Cargar archivo", wxPoint(155, 50), wxSize(150, 50));

	btnGenReportes = new wxButton(panel, GENREPORTES_ID, "Generar reportes", wxPoint(325, 50), wxSize(150, 50));

	btnAnalyze = new wxButton(panel, ANALYZE_ID, "Analizar", wxPoint(495, 50), wxSize(150, 50));

	textArea = new wxTextCtrl(panel, TEXTAREA_ID, "", wxPoint(50, 120), wxSize(700, 200), wxTE_MULTILINE | wxTE_RICH);
};

void MainFrame::OnButtonLoadClicked(wxCommandEvent& evt) {
	wxFileDialog openFileDialog(this, _("Open file"), "", "",
		"MED files (*.med)|*.med", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return; // si no se escogió nada

	// carga del archivo
	filepath = openFileDialog.GetPath();
	wxLogMessage("Selected file: %s", filepath);

	std::string filename = filepath.ToStdString();

	if (filename.find(".med") != std::string::npos)
	{

	}
	else {
		wxLogMessage("El archivo no es valido, o no se selecciono uno.");
		return;
	}

	std::ifstream archivo(filename);

	if (!archivo.is_open()) {
		wxLogMessage("No se pudo abrir el archivo");
		return;
	}

	std::stringstream buffer;

	buffer << archivo.rdbuf();
	wxString contenido = buffer.str();

	textArea->SetValue(wxString(buffer.str().c_str(), wxConvUTF8));

};

void MainFrame::OnButtonAnalyzeClicked(wxCommandEvent& evt) {
	
	if (lexicalAnalyzer != nullptr) {
		lexicalAnalyzer->nextToken(filepath);

		textArea->SetValue("");
		textArea->SetValue("TOKENS ENCONTRADOS \n\n");

		for (const auto& token : lexicalAnalyzer->tokens) {
			
			textArea->AppendText(wxString::Format("Token: %s, Lexema: %s, Linea: %d, Columna: %d\n",
				token.getTypeString(), token.lexeme, token.line, token.column));
		}

		textArea->AppendText("ERRORES ENCONTRADOS \n\n");

		for (const auto& error : lexicalAnalyzer->errorManager->getErrores()) {
			wxString errorMsg = wxString::Format("Error: %s, Linea: %d, Columna: %d, Gravedad: %s, Descripcion: %s\n",
				wxString::FromUTF8(error.getStringTipoError().c_str()),
				error.line,
				error.column,
				wxString::FromUTF8(error.getStringGravedad().c_str()),
				wxString::FromUTF8(error.description.c_str())
			);
			textArea->AppendText(errorMsg);
		}



	}


	
};



void MainFrame::OnButtonGenReportesClicked(wxCommandEvent& evt) {

	if (lexicalAnalyzer->tokens.size()==0)
	{
		return;
	}

	reportGenerator->setTokens(lexicalAnalyzer->getTokens());
	reportGenerator->setErrors(lexicalAnalyzer->errorManager->getErrores());
	reportGenerator->parsearTokens(lexicalAnalyzer->getTokens());
	reportGenerator->cruzarDatos();
	reportGenerator->generateReporte1("reporte1_pacientes.html");
	reportGenerator->generateReporte2("reporte2_medicos.html");
	reportGenerator->generateReporte3("reporte3_citas.html");
	reportGenerator->generateReporte4("reporte4_estadistico.html");
	
};

void MainFrame::setImage(const wxString& imagePath) {
	
	
};

