#pragma once

//#include "Mustache/mustache.hpp"

#include "Cursor.h"

#include "Namespace.h"

#include "Mustache/mustache.hpp"

namespace Utils {

	void ToString(const CXString& str, std::string& output);

	//kainjow::mustache::data::type TemplateBool(bool value);

	kainjow::mustache::data::type TemplateBool(bool value);

	std::string GetQualifiedName(const CursorType& type);

	void LoadText(const std::string& fileName, std::string& output);

	void WriteText(const std::string& fileName, const std::string& text);

	void FatalError(const std::string& error);

	std::string GetQualifiedName(const std::string& displayName, const Namespace& currentNamespace);

	std::string GetQualifiedName(const Cursor& cursor, const Namespace& currentNamespace);
}