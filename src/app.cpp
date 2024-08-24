#include "app.h"
#include "mainframe.h"
#include <wx/wx.h>

bool App::OnInit()
{
    MainFrame *mainFrame = new MainFrame("Merge Video");
    mainFrame->SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
    mainFrame->SetIcon(wxIcon(""));
    mainFrame->Show();
    return true;
}
