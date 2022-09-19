#pragma once

#include "Cursor.h"
#include "Namespace.h"

class Invokable
{
public:
	using Signature = std::vector<std::string>;

	Invokable(const Cursor& cursor);

protected:
	//return type
	std::string m_ReturnType;

	//arguments
	Signature m_Signature;
};

const auto kRetureTypeVoid = "void";