#pragma once
#include <exception>
#include <string>

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

