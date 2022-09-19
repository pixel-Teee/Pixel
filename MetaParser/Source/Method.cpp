#include "PreCompiled.h"

#include "Header/Method.h"
#include "Header/Class.h"
#include "Header/MetaDataConfig.h"
#include "Header/MetaUtils.h"

Method::Method(const Cursor& cursor, const Namespace& currentNamespace, Class* parent /*= nullptr*/)
	:LanguageType(cursor, currentNamespace)
	,Invokable(cursor)
	,m_IsConst(cursor.IsConst())
	,m_Parent(parent)
	,m_Name(cursor.GetSpelling())
{
	m_IsOverload = false;
}

bool Method::ShouldCompile() const
{
	return IsAccessible();
}

kainjow::mustache::data Method::CompileTemplate(const ReflectionParser* context) const
{
	kainjow::mustache::data data{ kainjow::mustache::data::type::object };

	data["name"] = m_Name;

	data["parentQualifiedName"] = m_Parent->m_QualifiedName;

	data["qualifiedSignature"] = GetQualifiedSignature();

	data["returnType"] = m_ReturnType;

	std::cout << "IsOverload:";
	std::cout << m_IsOverload;
	std::cout << "IsOverload" << std::endl;

	data["isOverloadAndConst"] = Utils::TemplateBool(m_IsOverload && m_IsConst);//overload and const

	data["isOverload"] = Utils::TemplateBool(m_IsOverload && !m_IsConst);//just overload

	data["hasBracket"] = Utils::TemplateBool(m_IsOverload || m_IsConst);

	//argument list
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

std::string Method::GetName()
{
	return m_Name;
}

void Method::SetOverload(bool value)
{
	m_IsOverload = value;
}

bool Method::IsAccessible() const
{
	//if (m_accessModifier != CX_CXXPublic)
		//return false;
	//if the parent wants white listed method, then we must have the enable flag
	if (m_Parent->GetMetaData().GetFlag(nativeProperty::WhiteListMethods))
		return m_MetaData.GetFlag(nativeProperty::Enable);

	//must not be explicitly disabled
	return m_MetaData.GetFlag("HaveAnnotate") && !m_MetaData.GetFlag(nativeProperty::Disable);
}

std::string Method::GetQualifiedSignature() const
{
	//argument list
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

	std::string constNess = m_IsConst ? " const" : "";

	return m_ReturnType + "(" + m_Parent->m_QualifiedName + "::*)(" + argumentList + ")" + constNess;
}

