#include "PreCompiled.h"

#include "Header/Class.h"
#include "Header/Field.h"
#include "Header/MetaDataConfig.h"
#include "Header/MetaUtils.h"

Field::Field(const Cursor& cursor, const Namespace& currentNamespace, Class* parent /*= nullptr*/)
	:LanguageType(cursor, currentNamespace)
	,m_IsConst(cursor.GetType().IsConst())
	,m_parent(parent)
	,m_Name(cursor.GetSpelling())
	,m_Type(Utils::GetQualifiedName(cursor.GetType()))
{
	auto displayName = m_MetaData.GetNativeString(nativeProperty::DisplayName);

	if (displayName.empty())
		m_DisplayName = m_Name;
	else
		m_DisplayName = displayName;

	m_ExplicitGetter = m_MetaData.GetNativeString(nativeProperty::ExplicitGetter);
	m_VeryExplicitGetter = m_MetaData.GetNativeString(nativeProperty::VeryExplicitGetter);
	m_HasExplicitGetter = !m_ExplicitGetter.empty() || !m_VeryExplicitGetter.empty();
	
	m_ExplicitSetter = m_MetaData.GetNativeString(nativeProperty::ExplicitSetter);
	//m_VeryExplicitSetter = m_MetaData.GetNativeString(nativeProperty::VeryExplicitSetter);
	m_HasExplicitSetter = !m_ExplicitSetter.empty();
}

bool Field::ShouldCompile() const
{
	return IsAccessible();
}

kainjow::mustache::data Field::CompileTemplate(const ReflectionParser* context) const
{
	kainjow::mustache::data data = { kainjow::mustache::data::type::object };

	data["name"] = m_Name;
	data["displayName"] = m_DisplayName;
	data["type"] = m_Type;

	data["hasParent"] = Utils::TemplateBool(m_parent != nullptr);

	data["parentQualifiedName"] = m_parent->m_QualifiedName;

	data["isGetterAccessible"] = Utils::TemplateBool(IsGetterAccessible());
	data["hasExplicitGetter"] = Utils::TemplateBool(m_HasExplicitGetter);

	if (m_HasExplicitGetter)
	{
		std::string explicitGetter;

		if (m_parent && m_VeryExplicitGetter.empty())
		{
			explicitGetter = "&" + m_parent->m_QualifiedName + "::" + m_ExplicitGetter;
		}
		else
		{
			explicitGetter = m_VeryExplicitGetter;
		}

		data["explicitGetter"] = explicitGetter;
	}

	//data.set("getterBody", context->LoadTemplate())

	//data["getterBody"] =

	data["isSetterAccessible"] = Utils::TemplateBool(IsSetterAccessible());
	data["hasExplicitSetter"] = Utils::TemplateBool(m_HasExplicitSetter);
	data["explicitSetter"] = m_ExplicitSetter;

	m_MetaData.CompileTemplateData(data, context);

	return data;
}

bool Field::IsAccessible() const
{
	//return (m_HasExplicitGetter || m_HasExplicitSetter || (m_accessModifier == CX_CXXPublic && !m_MetaData.GetFlag(nativeProperty::Disable)));
	return m_MetaData.GetFlag("HaveAnnotate") && !m_MetaData.GetFlag(nativeProperty::Disable);
}

bool Field::IsGetterAccessible() const
{
	return m_HasExplicitGetter || m_accessModifier == CX_CXXPublic;
}

bool Field::IsSetterAccessible() const
{
	return m_HasExplicitSetter || (!m_IsConst && m_accessModifier == CX_CXXPublic);
}
 