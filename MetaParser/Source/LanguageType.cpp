#include "PreCompiled.h"

#include "Header/LanguageType.h"
#include "Header/MetaDataConfig.h"

LanguageType::LanguageType(const Cursor& cursor, const Namespace& currentNamespace)
	:m_MetaData(cursor),
	 m_Enabled(m_MetaData.GetFlag(nativeProperty::Enable))
	,m_ptrTypeEnabled(m_MetaData.GetFlag(nativeProperty::EnablePtrType))
	,m_constPtrTypeEnabled(m_MetaData.GetFlag(nativeProperty::EnableConstPtrType))
	,m_accessModifier(cursor.GetAccessModifier())
	,m_RootCursor(cursor)
{
}

const MetaDataManager& LanguageType::GetMetaData() const
{
	return m_MetaData;
}

std::string LanguageType::GetSourceFile() const
{
	return m_RootCursor.GetSourceFile();
}
