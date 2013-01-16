#include <string>
#include <iostream>
#include <vector>

#include "workspace.hpp"
#include "contentItem.hpp"
#include "utils.hpp"
#include "xmlDoc.hpp"

// OMEX manifest formats that we know about and can handle
// can usually handle the general case, so don't need the version specific URI's...
#define OMEX_MANIFEST_FORMAT "http://identifiers.org/combine.specifications/omex-manifest"
#define CELLML_FORMAT "http://identifiers.org/combine.specifications/cellml"
#define SEDML_FORMAT "http://identifiers.org/combine.specifications/sed-ml"
#define RDFXML_FORMAT "application/rdf+xml"
#define PNG_FORMAT "image/png"

Workspace::Workspace(const std::string &url, const std::string &id) : mUrl(url), mId(id)
{
    std::cout << "creating workspace for: " << mUrl.c_str() << std::endl;
    std::string manifestUrl = mUrl[mUrl.length()-1] == '/' ? mUrl : mUrl + "/";
    manifestUrl += "manifest.xml";
    std::string manifest = getUrlContent(manifestUrl.c_str());
    if (manifest.size() > 0)
    {
        // we have a manifest so we can handle the workspace.
        XmlDoc xml;
        xml.parseString(manifest);
        mContentItems = xml.processManifest();
        for (int i=0; i < mContentItems.size(); ++i)
        {
            std::cout << "Content item: " << i+1 << "\n";
            std::cout << "**  location: " << mContentItems[i]->getLocation().c_str() << "\n";
            std::cout << "**    format: " << mContentItems[i]->getFormat().c_str() << "\n";

        }
    }
    else
    {
        std::cout << "Ignoring workspace without a manifest.\n";
    }
}

Workspace::~Workspace()
{
    for (int i = 0; i < mContentItems.size(); ++i)
    {
        ContentItem* item = mContentItems.back();
        mContentItems.pop_back();
        delete item;
    }
}

const char* Workspace::getUrl() const
{
    return mUrl.c_str();
}

const char* Workspace::getId() const
{
    return mId.c_str();
}
