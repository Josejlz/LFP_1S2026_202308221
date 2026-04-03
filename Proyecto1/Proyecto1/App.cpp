#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>
#include "LexicalAnalyzer.h"

bool App :: OnInit() {
	MainFrame* mainFrame = new MainFrame("MedLexer");
	mainFrame->SetClientSize(800, 600);
	mainFrame->Center();
	mainFrame->Show();
	mainFrame->setLexicalAnalyzer(new LexicalAnalyzer());
	return true;
}

wxIMPLEMENT_APP(App);