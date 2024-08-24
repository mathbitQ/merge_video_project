#ifndef MEDIALIST_H
#define MEDIALIST_H

#include "mediafile.h"
#include <vector>
#include <cstdint>

class MediaList
{
public:
    MediaList();
    virtual ~MediaList();
    void push(MediaFile* index);
    void clear();
    void pop();
    void pop(uint64_t index);
    bool isEmpty();
    void display();
    MediaFile *at(uint64_t index);
    int size();

private:
    std::vector<MediaFile*> mediaFileList;
};

#endif // MEDIALIST_H
