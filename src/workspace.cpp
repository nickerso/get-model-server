#include <string>
#include <iostream>
#include <vector>

#include "workspace.hpp"
#include "contentItem.hpp"
#include "utils.hpp"
#include "xmlDoc.hpp"

Workspace::Workspace(const std::string &url) : mUrl(url)
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
