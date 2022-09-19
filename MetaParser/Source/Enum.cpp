#include "PreCompiled.h"

#include "Header/Enum.h"
#include "Header/MetaDataConfig.h"
#include "Header/MetaUtils.h"

Enum::Value::Value(Enum* parent, const Cursor& cursor)
	:key(cursor.GetDisplayName())//enum's name
	,value(parent->m_QualifiedName + "::" + key)
{
}

Enum::Enum(const Cursor& cursor, const Namespace& currentNamespace)
	:LanguageType(cursor, currentNamespace)
	,m_Name(cursor.GetType().GetDisplayName())
	,m_QualifiedName(m_Name)
{
	auto displayName = m_MetaData.GetNativeString(nativeProperty::DisplayName);

	if (displayName.empty())
		m_DisplayName = m_QualifiedName;
	else
		m_DisplayName = Utils::GetQualifiedName(cursor, currentNamespace);

	//it's an anonymous enum?
	if (m_DisplayName.find("anonymous enum") != std::string::npos)
		m_DisplayName = "";

	for (auto& child : cursor.GetChildren())
	{
		if (child.GetKind() == CXCursor_EnumConstantDecl)
		{
			MetaDataManager valueMeta(child);

			//don't add disabled values
			if (!valueMeta.GetFlag(nativeProperty::Disable))
				m_Values.emplace_back(this, child);
		}
	}
}

bool Enum::ShouldCompile() const
{
	return IsAccessible();
}

kainjow::mustache::data Enum::CompileTemplate(const ReflectionParser* context) const
{
	kainjow::mustache::data data{ kainjow::mustache::data::type::object };

	data["displayName"] = m_DisplayName;
	data["qualifiedName"] = m_QualifiedName;
	
	kainjow::mustache::data members{ kainjow::mustache::data::type::list };

	int32_t i = 0;

	for (auto& value : m_Values)
	{
		kainjow::mustache::data member{ kainjow::mustache::data::type::object };

		member["key"] = value.key;
		member["value"] = value.value;
		member["isLast"] = Utils::TemplateBool(i == m_Values.size() - 1);

		members << member;

		++i;
	}

	data.set("member", members);

	m_MetaData.CompileTemplateData(data, context);

	return data;
}

bool Enum::IsAccessible() const
{
	return m_MetaData.GetFlag(nativeProperty::Enable);
}
