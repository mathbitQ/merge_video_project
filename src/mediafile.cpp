#include "mediafile.h"
#include <wx/wx.h>
#include <iostream>

MediaFile::MediaFile()
{

}

MediaFile::MediaFile(const MediaFile& source)
{
    this->pathFile = source.pathFile;
    this->duration = source.duration;
}

MediaFile::MediaFile(wxString path, int64_t duration)
{
    this->pathFile = path;
    this->duration = duration;
}

MediaFile::~MediaFile()
{

}

void MediaFile::setPath(wxString path)
{
    this->pathFile = path;
}

wxString MediaFile::getPath()
{
    return this->pathFile;
}

void MediaFile::setDuration(int64_t duration)
{
    this->duration = duration;
}

int64_t MediaFile::getDuration()
{
    return this->duration;
}

void MediaFile::display()
{
    std::cout<<this->pathFile<<" "<<this->duration<<" seconds"<<std::endl;
}
