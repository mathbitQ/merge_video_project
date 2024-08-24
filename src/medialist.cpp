#include "medialist.h"

MediaList::MediaList()
{

}

MediaList::~MediaList()
{

}

void MediaList::push(MediaFile *index)
{
    mediaFileList.push_back(index);
}

void MediaList::clear()
{
    for(uint64_t i = 0; i < mediaFileList.size(); i++)
        delete mediaFileList[i];
    mediaFileList.clear();
}

bool MediaList::isEmpty()
{
    return mediaFileList.empty();
}

void MediaList::pop()
{
    delete mediaFileList.back();
    mediaFileList.pop_back();
}

void MediaList::pop(uint64_t index)
{
    if(index >= 0 && index <= mediaFileList.size() - 1)
    {
        delete mediaFileList.at(index);
        mediaFileList.erase(mediaFileList.begin() + index);
    }
    //For debug
    else
        std::cerr<<"Invalid position!"<<std::endl;
}

void MediaList::display()
{
    for(uint64_t i = 0; i < mediaFileList.size(); i++)
        mediaFileList[i]->display();
}

MediaFile *MediaList::at(uint64_t index)
{
    if((index >= 0 && index <= mediaFileList.size() - 1) || (mediaFileList.size() > 0))
        return mediaFileList.at(index);
    return nullptr;
}

int MediaList::size()
{
    return mediaFileList.size();
}
