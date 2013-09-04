#ifndef XMLDOC_HPP
#define XMLDOC_HPP

class ContentItem;

class XmlDoc
{
public:
    XmlDoc();
    ~XmlDoc();
    int parseString(const std::string& data);
    std::vector<ContentItem*> processManifest();

private:
    void* mXmlDocPtr;
};

#endif // XMLDOC_HPP
