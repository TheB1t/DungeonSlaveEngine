#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <d3d10.h>
#include <D3DX10.h>

#include "log.hpp"
#include "object.hpp"

class ObjParser {
    public:
        std::vector<Object> objects;

        ObjParser(const std::string& filename);

    private:
        Log log;
};