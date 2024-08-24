#include "informationdialog.h"
#include <wx/wx.h>
#include <wx/wxprec.h>


InformationDialog::InformationDialog(wxWindow *parent,
                                     wxWindowID id,
                                     const wxString& title,
                                     const wxPoint &pos,
                                     const wxSize &size,
                                     long style,
                                     const wxString& name) :
wxDialog(parent, id, title, pos, size, style, name)
{
    wxPanel *dialogPanel = new wxPanel(this, wxID_ANY);
    description = new wxStaticText(dialogPanel, wxID_ANY, "");
    wxButton *btnClose = new wxButton(dialogPanel, wxID_ANY, "Close");

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(description, wxSizerFlags().Left().Border(wxLEFT | wxTOP | wxRIGHT, 15));
    vbox->AddStretchSpacer(1);
    vbox->Add(btnClose, wxSizerFlags().Right().Border(wxRIGHT | wxBOTTOM, 15));

    btnClose->Bind(wxEVT_BUTTON, &InformationDialog::OnCloseButtonClicked, this);
    dialogPanel->SetSizer(vbox);
}

InformationDialog::~InformationDialog()
{

}

void InformationDialog::SetDescription(wxString desc)
{
    description->SetLabel(desc);
}

void InformationDialog::OnCloseButtonClicked(wxCommandEvent &evt)
{
    Close(true);
}
