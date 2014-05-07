#ifndef BIOMAPS_HPP
#define BIOMAPS_HPP

class Biomaps
{
public:
    Biomaps(const std::string& repositoryRoot);
    ~Biomaps();

private:
    Biomaps();
    std::string mRepositoryRoot;
};

#endif // BIOMAPS_HPP
