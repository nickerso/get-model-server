#ifndef CONTENTITEM_HPP
#define CONTENTITEM_HPP

class ContentItem
{
public:
    ContentItem();
    void setLocation(const std::string& location);
    void setLocation(const char* location);
    const std::string& getLocation();
    void setFormat(const std::string& format);
    void setFormat(const char* format);
    const std::string& getFormat();

private:
    std::string mLocation;
    std::string mFormat;
};

#endif // CONTENTITEM_HPP
