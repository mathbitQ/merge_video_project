#include "mainframe.h"

// Get description defined for information dialog
#define INFORMATIONDIALOG \
"NOTE 1: Merge software only supports mp4 files with h264 codecs.\n"\
"Remember when merging video, you should have all the videos had\n"\
"the same codecs, frame rate, bit rate and resolution.\n"\
"NOTE 2: Constant bit rate might lower your video quality, so use it wisely.\n\n\n"\
"Recommeded video bit rate (Source from YouTube Help Center)\n"\
"Recommendation for bit rate (SDR): \n"\
"Based on 24, 25, 30 fps (Standard Frame Rate); 48, 50, 60 fps (High Frame Rate)\n"\
"4320p (8K)  : 80 - 160 Mbps; 120 - 240 Mbps\n"\
"2160p (4K)  : 35 - 45 Mbps; 53 - 68 Mbps\n"\
"1440p (2K)  : 16 Mbps; 24 Mbps\n"\
"1080p (FHD) : 8 Mbps; 12 Mbps\n"\
"720p  (HD)  : 5 Mbps; 7.5 Mbps\n"\
"480p  (SD)  : 2.5 Mbps; 4 Mbps\n"\
"360p  (LSD) : 1 Mbps; 1.5 Mbps\n\n"\
"Recommendation for bit rate (HDR): \n"\
"Based on 24, 25, 30 fps (Standard Frame Rate); 48, 50, 60 fps (High Frame Rate)\n"\
"4320p (8K)  : 100 - 200 Mbps; 150 - 300 Mbps\n"\
"2160p (4K)  : 44 - 56 Mbps; 66 - 85 Mbps\n"\
"1440p (2K)  : 20 Mbps; 30 Mbps\n"\
"1080p (FHD) : 10 Mbps; 15 Mbps\n"\
"720p  (HD)  : 6.5 Mbps; 9.5 Mbps\n"\
"480p  (SD)  : X; X\n"\
"360p  (LSD) : X; X\n\n"\
"Recommendation for audio bit rates: \n"\
"Mono: 128 kbps \n"\
"Stereo:  384 kbps\n"\
"5.1:  512 kbps\n"\
"\nVersion: 1.0 (Library: wxWidgets, ffmpeg. Code: C/C++)"

wxIMPLEMENT_APP(App);

bool filterExtension(const wxString filename, const wxString extension)
{
    size_t foundIndex = filename.find_last_of(".");
    wxString filenameExtension = filename.substr(foundIndex);
    if(filenameExtension.compare(extension) == 0)
        return true;
    return false;
}

wxString getFileNameWExtension(const wxString path)
{
    size_t getDirectoryIndex;
    #ifdef WIN32
        getDirectoryIndex = path.find_last_of("\\");
    #endif // WIN32
    #ifdef linux
        getDirectoryIndex = path.find_last_of("/");
    #endif // linux
    return path.substr(getDirectoryIndex + 1);
}

wxString getFileName(const wxString filename)
{
    size_t getExtension = filename.find_last_of(".");
    return filename.substr(0, getExtension);
}

// Scan for processed video
bool isScanned(int* list, int size)
{
    for(int i = 0; i < size; i++)
        if(list[i] == 0)
            return false;
    return true;
}

//This will generate ffmpeg command for processing video
//Command: ffmpeg -i <path_to_file> [-b:v <amount>k|M](optional) [-maxrate <amount>k|M](optional) [-bufsize <amount>k|M](optional) [-r <amount>](optional) -c:v libx264 <output_file>
wxString setAllSettingMedia(wxString inputMedia, wxString outputMedia, double bitrate, int bitrateType, int framerate, bool isConstantBitrate, double bufSize)
{
    wxString command = FFMPEG_INPUT;
    // File name should be inside single quotation in case there are whitespace filename
    // Input file
    command = command + (wxString) "\"" + inputMedia + (wxString) "\" ";
    // Bit rate
    if(bitrate != 0)
    {
        command = command + (wxString) BIT_RATE;
        if(bitrateType == 0) //As kbps
            command = command + (wxString) std::to_string(bitrate) + (wxString) "k ";
        else //bitrateType == 1 as Mbps
            command = command + (wxString) std::to_string(bitrate) + (wxString) "M ";
    }
    // Use constant bit rate and buffer size
    if(isConstantBitrate)
    {
        command = command + (wxString) MAX_BIT_RATE;
        if(bitrateType == 0) //As kbps
            command = command + (wxString) std::to_string(bitrate) + (wxString) "k "
                              + BUFFER_SIZE + (wxString) std::to_string(bufSize) + (wxString) "k ";
        else //bitrateType == 1 as Mbps
            command = command + (wxString) std::to_string(bitrate) + (wxString) "M "
                              + BUFFER_SIZE + (wxString) std::to_string(bufSize) + (wxString) "M ";
    }
    // Use frame rate
    if(framerate != 0)
    {
        command = command + (wxString) FRAME_RATE + (wxString) std::to_string(framerate) + (wxString) " ";
    }
    // Output
    command = command + (wxString) "\"" + outputMedia + (wxString) "\"";

    return command;
}

//This will generate ffmpeg command for merging final product
//Command: ffmpeg -f concat -safe 0 -i <path_to_text_file> -c copy <output_file>
wxString mergeVideo(wxString inputTextFile, wxString outputMedia)
{
    wxString command = "ffmpeg ";
    command = command + (wxString) CONCAT + (wxString) NO_SAFE_CONCAT;
    command = command + (wxString)"-i \"" + inputTextFile + (wxString)"\" ";
    command = command + (wxString) VIDEO_COPY + (wxString)"\"" + outputMedia + (wxString) "\"";
    return command;
}

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
    fileItemListCount = 0;
    CreateComponent();
    SetupSizer();
    BindAction();
}

void MainFrame::CreateComponent()
{
    container = new wxPanel(this);
    wxFont listFont(wxFontInfo(wxSize(0, 15)));
    // Left panel for controlling list of videos
    // -- Begin of left panel
    leftPanel = new wxPanel(container, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

    // Initialize the button and label of opening folder and file
    labelFolder = new wxStaticText(leftPanel, wxID_ANY, "Folder");
    btnImportFolder = new wxButton(leftPanel, wxID_ANY, "Import");

    labelFile = new wxStaticText(leftPanel, wxID_ANY, "File");
    btnImportFile = new wxButton(leftPanel, wxID_ANY, "Add file");

    wxInitAllImageHandlers();

    wxImage informationImage(wxT("./resource/information.png"), wxBITMAP_TYPE_PNG);
    wxImage informationIcon = informationImage.Scale(16, 16);
    wxBitmap informationIconBitmap(informationIcon);
    if(informationIconBitmap.IsOk())
    {
        btnDisplayInfo = new wxButton(leftPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(24,24), wxBU_EXACTFIT);
        btnDisplayInfo->SetBitmap(informationIconBitmap);
    }
    else
    {
        btnDisplayInfo = new wxButton(leftPanel, wxID_ANY, "i");
    }
    // Create the table for displaying file location using List Controller
    listFileText = new wxStaticText(leftPanel, wxID_ANY, "Video List");
    listFileText->SetFont(listFont);
    listFile = new wxListView(leftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    listFile->AppendColumn("#");
    listFile->AppendColumn("Filename");
    listFile->AppendColumn("Duration");
    listFile->SetColumnWidth(0, 50);
    listFile->SetColumnWidth(1, 300);
    listFile->SetColumnWidth(2, 100);

    // Create button for clearing all files, removing a file and export
    btnClearFile = new wxButton(leftPanel, wxID_ANY, "Clear All");
    btnRemoveFile = new wxButton(leftPanel, wxID_ANY, "Remove");
    btnExportFile = new wxButton(leftPanel, wxID_ANY, "Export");
    btnExportFile->Disable();
    // -- End of left panel

    // Right panel for setting when merging videos, allows the user
    // to configure their own choices
    // -- Begin of right panel

    rightPanel = new wxPanel(container);
    rightTopPanel = new wxPanel(rightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    rightBottomPanel = new wxPanel(rightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    settingText = new wxStaticText(rightTopPanel, wxID_ANY, "Setting");
    // Number of video
    numberVideoText = new wxStaticText(rightTopPanel, wxID_ANY, "Number of videos to create (1 - 100)");
    spinCtrlNumberVideo = new wxSpinCtrl(rightTopPanel);
    spinCtrlNumberVideo->SetMin(1);
    spinCtrlNumberVideo->SetMax(100);

    // Merge video
    randomMergeText = new wxStaticText(rightTopPanel, wxID_ANY, "Random Merge");
    rangeCharacter = new wxStaticText(rightTopPanel, wxID_ANY, "-");
    minNumberMergeVideo = new wxSpinCtrl(rightTopPanel);
    minNumberMergeVideo->SetMin(0);
    minNumberMergeVideo->SetMax(100);

    maxNumberMergeVideo = new wxSpinCtrl(rightTopPanel);
    maxNumberMergeVideo->SetMin(0);
    maxNumberMergeVideo->SetMax(100);

    // Bitrate for merged video
    // Choice added to bitrate unit conversion
    wxArrayString choices;
    choices.Add("kbps");
    choices.Add("Mbps");

    bitrateText = new wxStaticText(rightTopPanel, wxID_ANY, "Bit rate");
    bitrateConversion = new wxComboBox(rightTopPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, -1), choices, wxCB_DROPDOWN);
    bitrateConversion->SetEditable(false);
    bitrateConversion->SetSelection(0);
    bitrateAmount = new wxSpinCtrlDouble(rightTopPanel);
    bitrateAmount->SetDigits(1);
    bitrateAmount->SetRange(0, 10000);
    bitrateBuffText = new wxStaticText(rightTopPanel, wxID_ANY, "Buffer size (based on bit rate unit)");
    bitrateBuffAmount = new wxSpinCtrlDouble(rightTopPanel);
    bitrateBuffAmount->SetDigits(1);
    bitrateBuffAmount->SetRange(0, 10000);
    framerateText = new wxStaticText(rightTopPanel, wxID_ANY, "Frame rate");
    framerateAmount = new wxSpinCtrl(rightTopPanel);
    framerateAmount->SetMin(0);
    framerateAmount->SetMax(60);

    bitrateBuffText->Hide();
    bitrateBuffAmount->Hide();

    // Include track list for export, named as "tracklist_<firstfilename>.txt"
    includeTrackList = new wxCheckBox(rightTopPanel, wxID_ANY, "Include track list");
    constantBitrate = new wxCheckBox(rightTopPanel, wxID_ANY, "Use constant bit rate");
    // Command on right bottom panel
    commandText = new wxStaticText(rightBottomPanel, wxID_ANY, "Command running: ");
    commandDisplay = new wxStaticText(rightBottomPanel, wxID_ANY, "");

    gaugeProgressText = new wxStaticText(rightBottomPanel, wxID_ANY, "Progress");
    gaugeProgress = new wxGauge(rightBottomPanel, wxID_ANY, 0);
    // -- End of right pane


}

void MainFrame::SetupSizer()
{
    // Start of left panel
    wxBoxSizer *boxSizerFolder = new wxBoxSizer(wxVERTICAL);
    boxSizerFolder->Add(labelFolder, wxSizerFlags());
    boxSizerFolder->AddSpacer(5);
    boxSizerFolder->Add(btnImportFolder, wxSizerFlags());


    wxBoxSizer *boxSizerFile = new wxBoxSizer(wxVERTICAL);
    boxSizerFile->Add(labelFile, wxSizerFlags());
    boxSizerFile->AddSpacer(5);
    boxSizerFile->Add(btnImportFile, wxSizerFlags());

    wxBoxSizer *boxSizerImport = new wxBoxSizer(wxHORIZONTAL);
    boxSizerImport->Add(boxSizerFolder, wxSizerFlags());
    boxSizerImport->AddSpacer(15);
    boxSizerImport->Add(boxSizerFile, wxSizerFlags());
    boxSizerImport->AddSpacer(15);
    boxSizerImport->Add(btnDisplayInfo, wxSizerFlags().Bottom());

    wxBoxSizer *boxSizerFileManager = new wxBoxSizer(wxHORIZONTAL);
    boxSizerFileManager->Add(btnClearFile, wxSizerFlags());
    boxSizerFileManager->AddSpacer(5);
    boxSizerFileManager->Add(btnRemoveFile, wxSizerFlags());
    boxSizerFileManager->AddSpacer(5);
    boxSizerFileManager->Add(btnExportFile, wxSizerFlags());

    wxBoxSizer *mainLeftSizer = new wxBoxSizer(wxVERTICAL);
    mainLeftSizer->Add(boxSizerImport, wxSizerFlags().Border(wxALL, 25));
    mainLeftSizer->AddSpacer(5);
    mainLeftSizer->Add(listFileText, wxSizerFlags().CentreHorizontal());
    mainLeftSizer->AddSpacer(5);
    mainLeftSizer->Add(listFile, wxSizerFlags().Border(wxLEFT | wxRIGHT, 25));
    mainLeftSizer->AddSpacer(25);
    mainLeftSizer->Add(boxSizerFileManager, wxSizerFlags().Right().Border(wxRIGHT | wxBOTTOM, 25));
    //End of left panel

    //Start of right panel

    wxBoxSizer *mergeSizer = new wxBoxSizer(wxHORIZONTAL);
    mergeSizer->Add(minNumberMergeVideo, wxSizerFlags());
    mergeSizer->AddSpacer(15);
    mergeSizer->Add(rangeCharacter, wxSizerFlags());
    mergeSizer->AddSpacer(15);
    mergeSizer->Add(maxNumberMergeVideo, wxSizerFlags());

    wxBoxSizer *bitrateSizer = new wxBoxSizer(wxHORIZONTAL);
    bitrateSizer->Add(bitrateText, wxSizerFlags().Centre());
    bitrateSizer->AddSpacer(5);
    bitrateSizer->Add(bitrateAmount, wxSizerFlags());
    bitrateSizer->AddSpacer(5);
    bitrateSizer->Add(bitrateConversion, wxSizerFlags());

    wxBoxSizer *bitrateBuffSizer = new wxBoxSizer(wxHORIZONTAL);
    bitrateBuffSizer->Add(bitrateBuffText, wxSizerFlags().Centre());
    bitrateBuffSizer->AddSpacer(5);
    bitrateBuffSizer->Add(bitrateBuffAmount, wxSizerFlags());

    wxBoxSizer *framerateSizer = new wxBoxSizer(wxHORIZONTAL);
    framerateSizer->Add(framerateText, wxSizerFlags().Centre());
    framerateSizer->AddSpacer(5);
    framerateSizer->Add(framerateAmount, wxSizerFlags());

    // Right top panel
    wxBoxSizer *topRightSizer = new wxBoxSizer(wxVERTICAL);

    topRightSizer->AddSpacer(25);
    topRightSizer->Add(settingText, wxSizerFlags().CentreHorizontal().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(numberVideoText, wxSizerFlags().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(spinCtrlNumberVideo, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(randomMergeText, wxSizerFlags().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(mergeSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(bitrateSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(bitrateBuffSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(framerateSizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(includeTrackList, wxSizerFlags().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(5);
    topRightSizer->Add(constantBitrate, wxSizerFlags().Border(wxLEFT | wxRIGHT, 15));
    topRightSizer->AddSpacer(25);
    rightTopPanel->SetSizer(topRightSizer);

    // Right bottom panel
    wxBoxSizer *bottomRightSizer = new wxBoxSizer(wxVERTICAL);
    bottomRightSizer->Add(commandText, wxSizerFlags().Border(wxLEFT | wxRIGHT, 15));
    bottomRightSizer->AddSpacer(5);
    bottomRightSizer->Add(commandDisplay, wxSizerFlags().Border(wxLEFT | wxRIGHT, 15));
    bottomRightSizer->AddSpacer(25);
    bottomRightSizer->Add(gaugeProgressText, wxSizerFlags().CentreHorizontal());
    bottomRightSizer->AddSpacer(5);
    bottomRightSizer->Add(gaugeProgress, wxSizerFlags().CentreHorizontal());
    bottomRightSizer->AddSpacer(25);
    rightBottomPanel->SetSizer(bottomRightSizer);

    wxBoxSizer *mainRightSizer = new wxBoxSizer(wxVERTICAL);
    mainRightSizer->Add(rightTopPanel, wxSizerFlags().Expand());
    mainRightSizer->Add(rightBottomPanel, wxSizerFlags().Expand());
    //End of right panel

    // Set sizer to main frame
    leftPanel->SetSizer(mainLeftSizer);
    rightPanel->SetSizer(mainRightSizer);
    wxBoxSizer *containerSizer = new wxBoxSizer(wxHORIZONTAL);
    containerSizer->Add(leftPanel, wxSizerFlags().Expand());
    containerSizer->Add(rightPanel, wxSizerFlags().Expand());

    container->SetSizer(containerSizer);
    containerSizer->SetSizeHints(this);
    this->Layout();
}

void MainFrame::BindAction()
{
    btnImportFolder->Bind(wxEVT_BUTTON, &MainFrame::OnImportFolderClicked, this);
    btnImportFile->Bind(wxEVT_BUTTON, &MainFrame::OnImportFileClicked, this);
    btnClearFile->Bind(wxEVT_BUTTON, &MainFrame::OnClearAllClicked, this);
    btnRemoveFile->Bind(wxEVT_BUTTON, &MainFrame::OnRemoveClicked, this);
    btnDisplayInfo->Bind(wxEVT_BUTTON, &MainFrame::OnInformationClicked, this);
    //Example
    btnExportFile->Bind(wxEVT_BUTTON, &MainFrame::OnExportExample, this);
    constantBitrate->Bind(wxEVT_CHECKBOX, &MainFrame::OnToggleBufferConstant, this);
    bitrateConversion->Bind(wxEVT_COMBOBOX, &MainFrame::OnConversionBitrateChanged, this);
    this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
}

// Capping the bitrate amount
void MainFrame::OnConversionBitrateChanged(wxCommandEvent &evt)
{
    wxString unitConversion = bitrateConversion->GetValue();
    if(unitConversion.compare("kbps") == 0)
    {
        bitrateAmount->SetRange(0, 10000);
        bitrateBuffAmount->SetRange(0, 10000);
    }
    else //Mbps
    {
        bitrateAmount->SetRange(0, 300);
        bitrateBuffAmount->SetRange(0, 300);
    }
}


void MainFrame::OnImportFolderClicked(wxCommandEvent &evt)
{
    wxDirDialog openFolder(this, "Choose a directory/folder to import MP4 file", wxEmptyString, wxDD_DIR_MUST_EXIST | wxDD_DEFAULT_STYLE);
    if(openFolder.ShowModal() == wxID_OK)
    {
        wxDir fileDir(openFolder.GetPath());

        wxString filename;
        bool cont = fileDir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);

        while(cont)
        {
            wxString fullpath(openFolder.GetPath());
            if(fullpath[fullpath.length() - 1] != '\\')
                fullpath.append("\\");
            fullpath.append(filename);
            int64_t duration;
            MediaFile* index;

            if(filterExtension(filename, ".mp4"))
            {
                fileItemListCount++;
                listFile->InsertItem(fileItemListCount - 1, std::to_string(fileItemListCount));
                listFile->SetItem(fileItemListCount - 1, 1, fullpath);
                duration = getMediaDuration(&fullpath);
                wxString durationLabel = getMediaTimerFormat(duration);
                listFile->SetItem(fileItemListCount - 1, 2, durationLabel);

                index = new MediaFile(fullpath, duration);
                mediaList.push(index);
            }
            cont = fileDir.GetNext(&filename);
        }
        if(fileItemListCount != 0)
            btnExportFile->Enable();
    }
}

void MainFrame::OnImportFileClicked(wxCommandEvent &evt)
{
    wxFileDialog openFile(this, "Choose a MP4 file to import", wxEmptyString, wxEmptyString, FILETYPE, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if(openFile.ShowModal() == wxID_OK)
    {
        wxArrayString pathFile;
        openFile.GetPaths(pathFile);
        for(const wxString& file : pathFile)
        {
            int64_t duration;
            fileItemListCount++;
            listFile->InsertItem(fileItemListCount - 1, std::to_string(fileItemListCount));
            listFile->SetItem(fileItemListCount - 1, 1, file);
            duration = getMediaDuration(&file);
            wxString durationLabel = getMediaTimerFormat(duration);
            listFile->SetItem(fileItemListCount - 1, 2, durationLabel);
            MediaFile* index = new MediaFile(file, duration);
            mediaList.push(index);
        }
        if(fileItemListCount != 0)
            btnExportFile->Enable();
    }
}

void MainFrame::OnClearAllClicked(wxCommandEvent &evt)
{
    if(fileItemListCount <= 0)
    {
        wxMessageDialog requestClearAll(this, "There is nothing to clear", "Clear all files");
        requestClearAll.ShowModal();
        return;
    }

    wxMessageDialog requestClearAll(this, "Do you want to clear all the list file?", "Clear all files", wxYES_NO | wxCANCEL);
    if(requestClearAll.ShowModal() == wxID_YES)
    {
        listFile->DeleteAllItems();
        fileItemListCount = 0;
    }
    //Clear all the item on the vector
    mediaList.clear();
    btnExportFile->Disable();
}

void MainFrame::OnRemoveClicked(wxCommandEvent &evt)
{
    int selectedIndex = listFile->GetFocusedItem();

    if(selectedIndex == wxNOT_FOUND)
    {
        return;
    }

    // Redefine the index on list file
    for(int i = selectedIndex; i < fileItemListCount; i++)
        listFile->SetItem(i, 0, std::to_string(i));
    listFile->DeleteItem(selectedIndex);
    mediaList.pop(selectedIndex);
    fileItemListCount--;
    if(fileItemListCount == 0)
        btnExportFile->Disable();
}

void MainFrame::OnInformationClicked(wxCommandEvent &evt)
{
    InformationDialog infoDialog(this, wxID_ANY, "Information", wxDefaultPosition, wxSize(600, 600));
    infoDialog.SetDescription(INFORMATIONDIALOG);
    infoDialog.ShowModal();
}

// Used for toggle all the buttons whenever the export button for video is clicked
void MainFrame::ToggleButton(bool toggle)
{
    if(toggle)
    {
        btnImportFile->Enable();
        btnImportFolder->Enable();
        btnClearFile->Enable();
        btnRemoveFile->Enable();
        btnExportFile->Enable();
    }
    else
    {
        btnImportFile->Disable();
        btnImportFolder->Disable();
        btnClearFile->Disable();
        btnRemoveFile->Disable();
        btnExportFile->Disable();
    }
    return;
}

void MainFrame::OnWindowClosed(wxCloseEvent &evt)
{
    if(this->onProcessing)
    {
        wxMessageDialog requestCloseWindow(this, "Task is running. Do you want to exit?", "Exit", wxYES_NO | wxCANCEL);
        if(requestCloseWindow.ShowModal() == wxID_YES)
        {
            evt.Veto();
            this->quitRequested = true;
            //evt.Skip();
        }
        else
        {
            return;
        }
    }
    else
    {
        this->Destroy();
    }
}

void MainFrame::OnToggleBufferConstant(wxCommandEvent &evt)
{
    if(constantBitrate->GetValue())
    {
        bitrateBuffText->Show();
        bitrateBuffAmount->Show();
    }
    else
    {
        bitrateBuffText->Show(false);
        bitrateBuffAmount->Show(false);
    }
    rightTopPanel->Layout();
    rightPanel->Layout();
    this->Layout();
}

// Main core of the merge video program
void MainFrame::OnExportExample(wxCommandEvent &evt)
{
    wxDirDialog exportLocation(this, "Choose a directory/path to export media file", wxEmptyString);
    if(exportLocation.ShowModal() == wxID_OK)
    {
        std::cout<<"Export path: "<<exportLocation.GetPath()<<std::endl;
    }
    else //The user changed the idea
    {
        return;
    }

    //Check whether the ffmpeg exists
    std::string currentFilePath = std::filesystem::current_path().string();
    std::string ffmpegDefine;
    #ifdef WIN32
    ffmpegDefine = "\\bin\\ffmpeg.exe ";
    #endif // WIN32
    #ifdef linux
    ffmpegDefine = "/bin/ffmpeg ";
    #endif // linux

    std::ifstream ffmpegExist(currentFilePath + ffmpegDefine);
    if(!ffmpegExist.is_open())
    {
        wxMessageDialog ffmpegExistDialog(this, "FFMPEG NOT FOUND! Please install ffmpeg from https://ffmpeg.org/download.html"
                                          " and put it into the '/bin' folder on this program!", "FFMPEG Not Found Error");
        ffmpegExistDialog.ShowModal();
        return;
    }
    ffmpegExist.close();

    // Main core of the program
    if(!this->onProcessing)
    {
        // Disable all the toggle button
        this->ToggleButton(false);
        // Enable processing to prevent the user from misclicking
        this->onProcessing = true;



        // Define all the values taken from the setting and file path
        wxString pathLocation = exportLocation.GetPath();
        int numOfVideo = spinCtrlNumberVideo->GetValue();
        int minMergeVideo = minNumberMergeVideo->GetValue();
        int maxMergeVideo = maxNumberMergeVideo->GetValue();
        int conversionType = bitrateConversion->GetCurrentSelection(); //0 for kbps and 1 for Mbps
        double bitAmount = bitrateAmount->GetValue(); //Can be used as constant bit rate and maximum bitrate
        int frAmount = framerateAmount->GetValue();
        bool isTrackList = includeTrackList->GetValue();
        bool isConstantBitrate = constantBitrate->GetValue();
        double bitrateBuffer = (isConstantBitrate) ? bitrateBuffAmount->GetValue() : -1;
        gaugeProgress->SetRange(numOfVideo);
        // Print information
        std::cout<<"Num video: "<<numOfVideo<<std::endl;
        std::cout<<"Min merge video: "<<minMergeVideo<<std::endl;
        std::cout<<"Max merge video: "<<maxMergeVideo<<std::endl;
        std::cout<<"Conversion Type: "<<conversionType<<std::endl;
        std::cout<<"Bitrate: "<<bitAmount<<std::endl;
        std::cout<<"Framerate: "<<frAmount<<std::endl;
        std::cout<<"Include track list: "<<isTrackList<<std::endl;
        std::cout<<"Use constant bitrate: "<<isConstantBitrate<<std::endl;
        std::cout<<"Bitrate buffer: "<<bitrateBuffer<<std::endl;
        std::cout<<"Content of the file: "<<std::endl;
        mediaList.display();


        std::cout << "1. Main thread: " << std::this_thread::get_id() << std::endl;
        const auto processingThread  = [this,
                                        numOfVideo,
                                        minMergeVideo,
                                        maxMergeVideo,
                                        conversionType,
                                        bitAmount,
                                        frAmount,
                                        isTrackList,
                                        isConstantBitrate,
                                        bitrateBuffer,
                                        pathLocation]()
        {
            std::cout <<"2. Children thread: "<<std::this_thread::get_id() << std::endl;
            // With multiple video to create
            // Set range for processing video
            wxGetApp().CallAfter([this, numOfVideo](){
                this->gaugeProgress->SetRange(numOfVideo);
                this->gaugeProgress->SetValue(0);
                this->Layout();
                                 });

            // These two lists will be used for storing path and its processed video
            int *processedMedia = new int[mediaList.size()]{0};
            MediaList processedMediaList;
            int maxMergeVideoCustom = maxMergeVideo;
            auto start = std::chrono::steady_clock::now();
            for(int i = 0; i < numOfVideo; i++)
            {
                MediaList outputMediaList;
                int numOfVideoMergeArg;

                if(this->quitRequested)
                {
                    wxGetApp().CallAfter([this](){
                        this->backgroundThread.join();
                        this->onProcessing = false;
                        this->quitRequested = false;
                        this->Destroy();
                                         });
                    return;
                }

                // Defines the applied video with custom list saved on an export directory
                int *usedMediaList = new int[mediaList.size()]{0}; // To avoid using list again when merging

                // Converts video with applied format
                /* --       Condition       --
                    1. If random merge is disable (minMergeVideo = 0 or maxMergeVideo = 0 or invalid minMergeVideo > maxMergeVideo), merge video in order with all the videos
                    2. If bit rate is set to 0, keep the default bit rate (buffer size won't include even if use constant bit rate is on)
                    3. If the frame rate is set to 0, keep the default frame rate
                    4. Include track list with format:
                        file '<path_to_file>'
                        file '<path_to_file>'
                    5. For bit rate, value of 0 represents kbps as 'k' and 1 represents Mbps as 'M'
                   --                       --*/

                if(minMergeVideo == 0 || maxMergeVideo == 0 || (minMergeVideo > maxMergeVideo))
                {
                    numOfVideoMergeArg = mediaList.size();
                }
                else
                {
                    srand(time(NULL));

                    // To avoid getting outside of index list
                    if(maxMergeVideoCustom > mediaList.size())
                    {
                        maxMergeVideoCustom = mediaList.size();
                    }
                    numOfVideoMergeArg = rand()%((maxMergeVideoCustom + 1)- minMergeVideo) + minMergeVideo;
                }

                // To avoid processing many time
                if(!isScanned(processedMedia, mediaList.size()))
                {

                    //There are two cases
                    //1. Random Merge is disabled (minMergeVideo = 0 or maxMergeVideo = 0 or invalid minMergeVideo > maxMergeVideo)
                    //2. Random Merge is enabled

                    // Case 1: Random Merge is disabled
                    if((minMergeVideo == 0 || maxMergeVideo == 0) || (minMergeVideo > maxMergeVideo))
                    {
                        for(int k = 0; k < numOfVideoMergeArg; k++)
                        {
                            wxString outputFile;
                            #ifdef WIN32
                                outputFile = pathLocation + (wxString) "\\"
                                                + getFileName(getFileNameWExtension(mediaList.at(k)->getPath()))
                                                + (wxString) "_modified.mp4";
                            #endif // WIN32
                            #ifdef linux
                                outputFile = pathLocation + (wxString) "/"
                                                + getFileName(getFileNameWExtension(mediaList.at(k)->getPath()))
                                                + (wxString) "_modified.mp4";
                            #endif // linux

                            MediaFile* outputFileIndex = new MediaFile(outputFile, mediaList.at(k)->getDuration());
                            outputMediaList.push(outputFileIndex);
                            usedMediaList[k] = 1;
                            if(processedMedia[k])
                            {
                                continue;
                            }
                            processedMediaList.push(new MediaFile(*outputFileIndex));
                            processedMedia[k] = 1;
                            // Generates command
                            wxString ffmpegCommandRun = setAllSettingMedia(mediaList.at(k)->getPath(),
                                                                  outputFile,
                                                                  bitAmount,
                                                                  conversionType,
                                                                  frAmount,
                                                                  isConstantBitrate,
                                                                  bitrateBuffer);

                            //Run command here for processing data
                            wxGetApp().CallAfter([this, ffmpegCommandRun](){
                                this->commandDisplay->SetLabel(ffmpegCommandRun);
                                this->Layout();
                                                 });
                            std::cout<<ffmpegCommandRun<<std::endl;
                            std::system(ffmpegCommandRun.ToUTF8().data());
                        }
                    }
                    // Case 2: Random Merge is enable
                    else
                    {
                        int scannedIndex = 0;
                        while(scannedIndex < numOfVideoMergeArg)
                        {
                            srand(time(NULL));
                            int randomIndex = rand()%mediaList.size();
                            if(usedMediaList[randomIndex])
                                continue;
                            wxString outputFile;
                            #ifdef WIN32
                                outputFile = pathLocation + (wxString) "\\"
                                                + getFileName(getFileNameWExtension(mediaList.at(randomIndex)->getPath()))
                                                + (wxString) "_modified.mp4";
                            #endif // WIN32
                            #ifdef linux
                                outputFile = pathLocation + (wxString) "/"
                                                + getFileName(getFileNameWExtension(mediaList.at(randomIndex)->getPath()))
                                                + (wxString) "_modified.mp4";
                            #endif // linux
                            MediaFile *outputFileIndex = new MediaFile(outputFile, mediaList.at(randomIndex)->getDuration());
                            outputMediaList.push(outputFileIndex);
                            usedMediaList[randomIndex] = 1;
                            if(processedMedia[randomIndex])
                            {
                                scannedIndex++;
                                continue;
                            }
                            processedMediaList.push(new MediaFile(*outputFileIndex));
                            processedMedia[randomIndex] = 1;
                            // Generates command
                            wxString ffmpegCommandRun = setAllSettingMedia(mediaList.at(randomIndex)->getPath(),
                                                                          outputFile,
                                                                          bitAmount,
                                                                          conversionType,
                                                                          frAmount,
                                                                          isConstantBitrate,
                                                                          bitrateBuffer);
                            //Run command here
                            wxGetApp().CallAfter([this, ffmpegCommandRun](){
                                this->commandDisplay->SetLabel(ffmpegCommandRun);
                                this->Layout();
                                                 });
                            std::cout<<ffmpegCommandRun<<std::endl;
                            std::system(ffmpegCommandRun.ToUTF8().data());
                            scannedIndex++;
                        }
                    }
                }
                else
                {
                    //Same as the processed media
                    //1. Random Merge is disabled (minMergeVideo = 0 or maxMergeVideo = 0 or invalid minMergeVideo > maxMergeVideo)
                    //2. Random Merge is enabled
                    if((minMergeVideo == 0 || maxMergeVideo == 0) || (minMergeVideo > maxMergeVideo))
                    {
                        for(int k = 0; k < numOfVideoMergeArg; k++)
                        {
                            wxString outputFile;
                            #ifdef WIN32
                                outputFile = pathLocation + (wxString) "\\"
                                                + getFileName(getFileNameWExtension(mediaList.at(k)->getPath()))
                                                + (wxString) "_modified.mp4";
                            #endif // WIN32
                            #ifdef linux
                                outputFile = pathLocation + (wxString) "/"
                                                + getFileName(getFileNameWExtension(mediaList.at(k)->getPath()))
                                                + (wxString) "_modified.mp4";
                            #endif // linux

                            MediaFile *outputFileIndex = new MediaFile(outputFile, mediaList.at(k)->getDuration());
                            outputMediaList.push(outputFileIndex);
                            usedMediaList[k] = 1;
                        }
                    }
                    else
                    {
                        int scannedIndex = 0;
                        while(scannedIndex < numOfVideoMergeArg)
                        {
                            srand(time(NULL));
                            int randomIndex = rand()%mediaList.size();
                            if(usedMediaList[randomIndex])
                                continue;
                            wxString outputFile;
                            #ifdef WIN32
                                outputFile = pathLocation + (wxString) "\\"
                                                + getFileName(getFileNameWExtension(mediaList.at(randomIndex)->getPath()))
                                                + (wxString) "_modified.mp4";
                            #endif // WIN32
                            #ifdef linux
                                outputFile = pathLocation + (wxString) "/"
                                                + getFileName(getFileNameWExtension(mediaList.at(randomIndex)->getPath()))
                                                + (wxString) "_modified.mp4";
                            #endif // linux
                            MediaFile *outputFileIndex = new MediaFile(outputFile, mediaList.at(randomIndex)->getDuration());
                            outputMediaList.push(outputFileIndex);
                            usedMediaList[randomIndex] = 1;
                            scannedIndex++;
                        }
                    }
                }
                // Adds to the text file only
                // The text file will have a name of video_merge_log.txt
                wxString mergeTextFileName;
                wxString outputFile;
                #ifdef WIN32
                    mergeTextFileName = pathLocation + (wxString) "\\video_merge_log.txt";
                    outputFile = pathLocation + (wxString) "\\"
                                    + getFileName(getFileNameWExtension(outputMediaList.at(0)->getPath()))
                                    + (wxString) "_final_"
                                    + (wxString) std::to_string(i)
                                    + (wxString) ".mp4";
                #endif // WIN32
                #ifdef linux
                    mergeTextFileName = pathLocation + (wxString) "/video_merge_log.txt";
                    outputFile = pathLocation + (wxString) "/"
                                    + getFileName(getFileNameWExtension(outputMediaList.at(0)->getPath()))
                                    + (wxString) "_final_"
                                    + (wxString) std::to_string(i)
                                    + (wxString) ".mp4";
                #endif // linux
                std::ofstream mergeTextFile(mergeTextFileName.ToUTF8().data());
                if(!mergeTextFile.is_open()) // Exit the thread if file can't be opened
                {
                    wxGetApp().CallAfter([this, mergeTextFileName](){
                        std::cout<<"Can't open file "<<mergeTextFileName<<std::endl;
                        this->commandDisplay->SetLabel("");
                        this->backgroundThread.join();
                        this->gaugeProgress->SetValue(0);
                        this->gaugeProgress->SetRange(0);
                        this->onProcessing = false;
                        this->ToggleButton(true);
                        this->Layout();
                                         });
                }
                for(int j = 0; j < outputMediaList.size(); j++)
                    mergeTextFile << "file '" << outputMediaList.at(j)->getPath() << "'\n";
                mergeTextFile.close();
                // Merge the final product
                wxString mergeCommand = mergeVideo(mergeTextFileName, outputFile);

                std::system(mergeCommand.ToUTF8().data());
                // Create a track list if the option is enable.
                // Name of a track list based on the first media file name
                if(isTrackList)
                {
                    wxString trackListName = (wxString) "Tracklist-"
                                           + (wxString) getFileName(getFileNameWExtension(outputMediaList.at(0)->getPath()))
                                           + (wxString) "_final_"
                                           + (wxString) std::to_string(i)
                                           + (wxString) ".txt";
                    wxString trackListPath;
                    #ifdef WIN32
                        trackListPath = pathLocation + (wxString) "\\" + trackListName;
                    #endif // WIN32
                    #ifdef linux
                        trackListPath = pathLocation + (wxString) "/" + trackListName;
                    #endif // linux

                    std::ofstream trackListWrite(trackListPath.ToUTF8().data());
                    uint64_t rootTimer = 0;
                    for(int j = 0; j < outputMediaList.size(); j++)
                    {
                        wxString getMediaName = getFileName(getFileNameWExtension(outputMediaList.at(j)->getPath()));
                        wxString getTimerFormat = getMediaTimerFormat(rootTimer);
                        trackListWrite << getTimerFormat << " " << getMediaName << "\n";
                        rootTimer = rootTimer + outputMediaList.at(j)->getDuration();
                    }
                    trackListWrite.close();
                }
                // Update the progress bar
                wxGetApp().CallAfter([this, i](){
                    this->gaugeProgress->SetValue(i + 1);
                    this->Layout();
                                     });
                delete [] usedMediaList;
                outputMediaList.clear();
                remove(mergeTextFileName.ToUTF8().data());
            }
            auto end = std::chrono::steady_clock::now();
            auto diff = end - start;

            std::cout<<"Time: "<<std::chrono::duration<double, std::milli>(diff).count()<<" [ms]"<<std::endl;


            //Deletes all the applied video to free memory, leaving final product
            for(int index = 0; index < processedMediaList.size(); index++)
            {
                remove(processedMediaList.at(index)->getPath().ToUTF8().data());
            }
            std::cout<<"Processed media list file path: "<<std::endl;
            processedMediaList.display();
            processedMediaList.clear();
            delete [] processedMedia;
            wxGetApp().CallAfter([this](){
                this->backgroundThread.join();
                this->onProcessing = false;
                this->ToggleButton(true);
                this->commandDisplay->SetLabel("");
                this->Layout();
                                 });
        };

        this->backgroundThread = std::thread{processingThread};
    }

}
