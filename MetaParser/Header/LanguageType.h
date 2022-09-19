#pragma once

#include "Cursor.h"
#include "MetaDataManager.h"
#include "Namespace.h"

class LanguageType
{
public:
	LanguageType(const Cursor& cursor, const Namespace& currentNamespace);

	virtual ~LanguageType() {}

	const MetaDataManager& GetMetaData() const;

	std::string GetSourceFile() const;

	virtual kainjow::mustache::data CompileTemplate(const ReflectionParser* context) const = 0;

protected:
	MetaDataManager m_MetaData;

	//determines if the type is enabled in reflection database generation
	bool m_Enabled;

	//determines if the pointer type to this type will be generated in the reflection data base
	bool m_ptrTypeEnabled;

	//determines if the constant pointer type to this type will be generated in the reflection database
	bool m_constPtrTypeEnabled;

	CX_CXXAccessSpecifier m_accessModifier;

public:
	//cursor that represents the root of the language type
	Cursor m_RootCursor;
};