#ifndef CONTENTITEM_HPP
#define CONTENTITEM_HPP

class ContentItem
{
public:
    ContentItem();
    void setLocation(const std::string& location);
    void setLocation(const char* location);
    const std::string& getLocation() const;
    void setFormat(const std::string& format);
    void setFormat(const char* format);
    const std::string& getFormat() const;

private:
    std::string mLocation;
    std::string mFormat;
};

#endif // CONTENTITEM_HPP
