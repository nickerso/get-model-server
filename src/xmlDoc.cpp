#include <string>
#include <iostream>
#include <vector>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "xmlDoc.hpp"
#include "utils.hpp"
#include "contentItem.hpp"
#include "namespaces.hpp"

static xmlNodeSetPtr executeXPath(xmlDocPtr doc, const xmlChar* xpathExpr);

class LibXMLWrapper
{
public:
    LibXMLWrapper()
    {
        std::cout << "initialise libxml\n";
        /* Init libxml */
        xmlInitParser();
        LIBXML_TEST_VERSION
    }
    ~LibXMLWrapper()
    {
        std::cout << "terminate libxml\n";
        /* Shutdown libxml */
        xmlCleanupParser();
    }
};

static LibXMLWrapper dummyWrapper;

XmlDoc::XmlDoc() : mXmlDocPtr(0)
{
}

XmlDoc::~XmlDoc()
{
    if (mXmlDocPtr)
    {
        xmlFreeDoc(static_cast<xmlDocPtr>(mXmlDocPtr));
    }
}

int XmlDoc::parseString(const std::string &data)
{
    xmlDocPtr doc = xmlParseMemory(data.c_str(), data.size());
    if (doc == NULL)
    {
        std::cerr << "Error parsing data string: **" << data.c_str() << "**\n";
        return -1;
    }
    mXmlDocPtr = static_cast<void*>(doc);
    return 0;
}

std::vector<ContentItem*> XmlDoc::processManifest()
{
    std::vector<ContentItem*> items;
    if (mXmlDocPtr == NULL)
    {
        std::cerr << "Can't process a manifest that hasn't been read in?!\n";
        return items;
    }
    xmlDocPtr doc = static_cast<xmlDocPtr>(mXmlDocPtr);
    xmlNodeSetPtr results = executeXPath(doc, BAD_CAST "//omf:content");
    if (results)
    {
        int i, n = xmlXPathNodeSetGetLength(results);
        for (i = 0; i < n; ++i)
        {
            xmlNodePtr node = xmlXPathNodeSetItem(results, i);
            if (node->type == XML_ELEMENT_NODE)
            {
                char* location = (char*)xmlGetProp(node, BAD_CAST "location");
                char* format = (char*)xmlGetProp(node, BAD_CAST "format");
                ContentItem* item = new ContentItem();
                items.push_back(item);
                item->setLocation(location);
                item->setFormat(format);
                xmlFree(location);
                xmlFree(format);
            }
        }
    }
    return items;
}

std::vector<std::pair<std::string, std::string> > XmlDoc::getCellmlComponentList()
{
    std::vector<std::pair<std::string, std::string> > componentList;
    if (mXmlDocPtr == NULL)
    {
        std::cerr << "Can't process a source document that hasn't been loaded?!\n";
        return componentList;
    }
    xmlDocPtr doc = static_cast<xmlDocPtr>(mXmlDocPtr);
    xmlNodeSetPtr results = executeXPath(doc, BAD_CAST "/cellml:model/cellml:component");
    if (results)
    {
        int i, n = xmlXPathNodeSetGetLength(results);
        for (i = 0; i < n; ++i)
        {
            xmlNodePtr node = xmlXPathNodeSetItem(results, i);
            if (node->type == XML_ELEMENT_NODE)
            {
                char* name = (char*)xmlGetProp(node, BAD_CAST "name");
                char* id = (char*)xmlGetProp(node, BAD_CAST "id");
                std::pair<std::string, std::string> p(name, id);
                componentList.push_back(p);
                xmlFree(name);
                xmlFree(id);
            }
        }
    }
    return componentList;
}

static xmlNodeSetPtr executeXPath(xmlDocPtr doc, const xmlChar* xpathExpr)
{
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xmlNodeSetPtr results = NULL;
    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL)
    {
        fprintf(stderr, "Error: unable to create new XPath context\n");
        return NULL;
    }
    /* Register namespaces */
    if (!(xmlXPathRegisterNs(xpathCtx, BAD_CAST CSIM_SIMULATION_PREFIX, BAD_CAST CSIM_SIMULATION_NS) == 0) ||
        !(xmlXPathRegisterNs(xpathCtx, BAD_CAST OMEX_MANIFEST_PREFIX, BAD_CAST OMEX_MANIFEST_NS) == 0) ||
        !(xmlXPathRegisterNs(xpathCtx, BAD_CAST CELLML_1_1_PREFIX, BAD_CAST CELLML_1_1_NS) == 0))
    {
        fprintf(stderr, "Error: unable to register namespaces\n");
        return NULL;
    }
    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if (xpathObj == NULL)
    {
        fprintf(stderr, "Error: unable to evaluate xpath expression \"%s\"\n",
                xpathExpr);
        xmlXPathFreeContext(xpathCtx);
        return NULL;
    }

    if (xmlXPathNodeSetGetLength(xpathObj->nodesetval) > 0)
    {
        int i;
        results = xmlXPathNodeSetCreate(NULL);
        for (i=0; i< xmlXPathNodeSetGetLength(xpathObj->nodesetval); ++i)
            xmlXPathNodeSetAdd(results, xmlXPathNodeSetItem(xpathObj->nodesetval, i));
    }
    /* Cleanup */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    return results;
}
