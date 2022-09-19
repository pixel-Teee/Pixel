#include "PreCompiled.h"

#include "Header/Constructor.h"

#include "Header/Class.h"

#include "Header/MetaDataConfig.h"

Constructor::Constructor(const Cursor& cursor, const Namespace& currentNamespace, Class* parent)
	:LanguageType(cursor, currentNamespace)
	, Invokable(cursor)
	, m_Parent(parent)
{

}

bool Constructor::ShouldCompile() const
{
	return IsAccessible();
}

kainjow::mustache::data Constructor::CompileTemplate(const ReflectionParser* context) const
{
	kainjow::mustache::data data{ kainjow::mustache::data::type::object };

	data["parentQualifiedName"] = m_Parent->m_QualifiedName;

	data["returnType"] = m_ReturnType;

	std::string argumentList;

	if (m_Signature.size() == 1)
	{
		argumentList = m_Signature[0];
	}
	else if (m_Signature.size() >= 2)
	{
		for (uint32_t i = 0; i < m_Signature.size() - 1; ++i)
		{
			argumentList = argumentList + m_Signature[i] + ", ";
		}

		argumentList += m_Signature.back();
	}

	data["argumentList"] = argumentList;

	m_MetaData.CompileTemplateData(data, context);

	return data;
}

bool Constructor::IsAccessible() const
{
	return m_MetaData.GetFlag("HaveAnnotate") && !m_MetaData.GetFlag(nativeProperty::Disable);
}

std::string Constructor::GetTemplateParameters(bool IsDynamic) const
{
	return std::string();
}

