#pragma once
#include <exception>
#include <string>

#ifdef NDEBUG
#define IS_DEBUG true
#else
#define IS_DEBUG false
#endif

class ChiliException
{
public:
	ChiliException(int line, const std::string file)noexcept;
	virtual const char* What()const noexcept;
	virtual const char* GetType()const noexcept;
	int GetLine()const noexcept;
	const std::string& GetFile()const noexcept;
	std::string GetOriginString()const noexcept;
private:
	int line;
	std::string file;
protected:
	mutable std::string WhatBuffer;
};

