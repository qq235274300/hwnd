#include "ChiliException.h"
#include <sstream>

ChiliException::ChiliException(int line, const std::string file) noexcept
    :line(line),file(file)
{
    
}

const char* ChiliException::What() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << GetOriginString();
    WhatBuffer = oss.str();
    return WhatBuffer.c_str();
}

const char* ChiliException::GetType() const noexcept
{
    return "ChiliException";
}

int ChiliException::GetLine() const noexcept
{
    return line;
}

const std::string& ChiliException::GetFile() const noexcept
{
    return file;
}

std::string ChiliException::GetOriginString() const noexcept
{
    std::stringstream oss;
    oss << "[Line] " << line << std::endl
        << "[File] " << file;
    return oss.str();
}
