#pragma once
#include "common.hpp"

namespace GitObjectCommon
{
    //Interfacing with database
    int addInObjects(string sha1Hash, string filecontents);
};