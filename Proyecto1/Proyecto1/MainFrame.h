#pragma once
#include <wx/wx.h>
#include <string>
#include <wx/string.h>
#include "LexicalAnalyzer.h"
#include "ReportGenerator.h"

class lexicalAnalyzer;
class ReportGenerator;

class MainFrame : public wxFrame
{
private:
	wxPanel* panel;

	wxButton* btnLoad;

	wxButton* btnGenReportes;

	wxButton* btnAnalyze;

	wxTextCtrl* textArea;

	void OnButtonLoadClicked(wxCommandEvent& evt);
	void OnButtonGenReportesClicked(wxCommandEvent& evt);
	void OnButtonAnalyzeClicked(wxCommandEvent& evt);
	LexicalAnalyzer* lexicalAnalyzer;
	ReportGenerator* reportGenerator;
	wxString filepath;
	wxDECLARE_EVENT_TABLE();

public:
	MainFrame(const wxString& title);
	void setLexicalAnalyzer(LexicalAnalyzer* la);
	void setReportGenerator(ReportGenerator* rg);
	void setImage(const wxString& imagePath);

};

