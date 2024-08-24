#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/font.h>
#include <wx/spinctrl.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/dir.h>
#include <wx/msgdlg.h>
#include <wx/wfstream.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <atomic>

#include "app.h"
#include "media_reader.h"
#include "informationdialog.h"
#include "medialist.h"
#include "ffmpeg_define.h"

#define FILETYPE "MP4 Files (*.mp4)|*.mp4"
#define FILESAVE "Text file (*.txt)|*.txt"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);
    void BindAction();
    void CreateComponent();
    void SetupSizer();
private:
    //List count and file list
    int fileItemListCount;
    MediaList mediaList;
    //Holding state
    bool onProcessing{false}; //For task running
    std::atomic<bool> quitRequested{false};
    std::thread backgroundThread;
    //Main panel container
    wxPanel *container;
    //Left Panel
    wxPanel *leftPanel;
    wxStaticText *labelFolder;
    wxButton *btnImportFolder;
    wxStaticText *labelFile;
    wxButton *btnImportFile;
    wxButton *btnDisplayInfo;
    wxStaticText *listFileText;
    wxListView *listFile;
    wxButton *btnClearFile;
    wxButton *btnRemoveFile;
    wxButton *btnExportFile;
    //Right Panel
    wxStaticText *settingText;
    wxPanel *rightPanel;
    wxPanel *rightTopPanel;
    wxStaticText *numberVideoText;
    wxSpinCtrl *spinCtrlNumberVideo;
    wxStaticText *randomMergeText;
    wxStaticText *rangeCharacter;
    wxSpinCtrl *minNumberMergeVideo;
    wxSpinCtrl *maxNumberMergeVideo;
    wxStaticText *bitrateText;
    wxComboBox *bitrateConversion;
    wxSpinCtrlDouble *bitrateAmount;
    wxStaticText *bitrateBuffText;
    wxSpinCtrlDouble *bitrateBuffAmount;
    wxStaticText *framerateText;
    wxSpinCtrl *framerateAmount;
    wxCheckBox *includeTrackList;
    wxCheckBox *constantBitrate;
    wxPanel *rightBottomPanel;
    wxStaticText *commandText;
    wxStaticText *commandDisplay;
    wxStaticText *gaugeProgressText;
    wxGauge *gaugeProgress;
    //Action
    void OnImportFileClicked(wxCommandEvent &evt);
    void OnImportFolderClicked(wxCommandEvent &evt);
    void OnClearAllClicked(wxCommandEvent &evt);
    void OnRemoveClicked(wxCommandEvent &evt);
    void OnExportExample(wxCommandEvent &evt);
    void OnInformationClicked(wxCommandEvent &evt);
    void ToggleButton(bool toggle);
    void OnWindowClosed(wxCloseEvent &evt);
    void OnToggleBufferConstant(wxCommandEvent &evt);
    void OnConversionBitrateChanged(wxCommandEvent &evt);
};


bool filterExtension(const std::string filename, const std::string extension);
#endif // MAINFRAME_H
