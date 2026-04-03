#pragma once
#include <wx/wx.h>
#include <string>
#include <wx/string.h>
#include "LexicalAnalyzer.h"

class lexicalAnalyzer;

class MainFrame : public wxFrame
{
private:
	wxPanel* panel;

	wxButton* btnLoad;

	wxButton* btnGenReportes;

	wxButton* btnAnalyze;

	wxTextCtrl* textArea;

	wxpngHandler* pngHandler;

	void OnButtonLoadClicked(wxCommandEvent& evt);
	void OnButtonGenReportesClicked(wxCommandEvent& evt);
	void OnButtonAnalyzeClicked(wxCommandEvent& evt);
	LexicalAnalyzer* lexicalAnalyzer;
	wxString filepath;
	wxDECLARE_EVENT_TABLE();

public:
	MainFrame(const wxString& title);
	void setLexicalAnalyzer(LexicalAnalyzer* la);
	void setImage(const wxString& imagePath) {
		wxImage image;
		if (image.LoadFile(imagePath)) {
			pngHandler->SetBitmap(wxBitmap(image));
		}
		else {
			wxLogMessage("No se pudo cargar la imagen");
		}
	}

};

