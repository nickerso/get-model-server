#include <string>
#include <iostream>

#include "contentItem.hpp"

ContentItem::ContentItem()
{
}

void ContentItem::setFormat(const std::string &format)
{
    mFormat = format;
}

void ContentItem::setFormat(const char* format)
{
    mFormat = std::string(format);
}

const std::string& ContentItem::getFormat() const
{
    return mFormat;
}

void ContentItem::setLocation(const std::string &location)
{
    mLocation = location;
}

void ContentItem::setLocation(const char* location)
{
    mLocation = std::string(location);
}

const std::string& ContentItem::getLocation() const
{
    return mLocation;
}
