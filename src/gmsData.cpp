#include <iostream>

#include "gmsData.hpp"

using namespace GMS;

Data::Data()
{
    std::cout << "Creating new GMS::Data for use in the GET model server." << std::endl;
}

Data::~Data()
{
    std::cout << "Destroying the GMS::Data from the GET model server." << std::endl;
}
