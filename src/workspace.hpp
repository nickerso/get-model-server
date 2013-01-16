#ifndef WORKSPACE_HPP
#define WORKSPACE_HPP

class ContentItem;

class Workspace
{
public:
    Workspace(const std::string& url);
    ~Workspace();
private:
    std::string mUrl;
    std::vector<ContentItem*> mContentItems;
};

#endif // WORKSPACE_HPP
