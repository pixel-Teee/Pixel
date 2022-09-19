#include "PreCompiled.h"

#include "Header/Class.h"
#include "Header/Field.h"
#include "Header/Method.h"
#include "Header/Constructor.h"
#include "Header/MetaDataConfig.h"
#include "Header/MetaUtils.h"

//TODO:need to fix our class
const auto kTypeObject = "ursine::meta::Object";
const auto kTypeMetaProperty = "ursine::meta::MetaProperty";

static const std::vector<std::string> nativeTypes
{
	kTypeObject,
	kTypeMetaProperty
};

static bool isNativeType(const std::string& qualifinedName)
{
	return std::find(
		nativeTypes.begin(),
		nativeTypes.end(),
		qualifinedName
	) != nativeTypes.end();
}

BaseClass::BaseClass(const Cursor& cursor)
	:name(cursor.GetType().GetCanonicalType().GetDisplayName())
{
	
}

Class::Class(const Cursor& cursor, const Namespace& currentNamespace)
	:LanguageType(cursor, currentNamespace)
	,m_Name(cursor.GetDisplayName())
	,m_QualifiedName(cursor.GetType().GetDisplayName())
{
	//from the meta data to get the display name
	//parameter is key
	//displayName is class name
	auto displayName = m_MetaData.GetNativeString(nativeProperty::DisplayName);

	if (displayName.empty())
	{
		m_DisplayName = m_QualifiedName;
	}
	else
	{
		m_DisplayName = displayName;
	}

	for (auto& child : cursor.GetChildren())
	{
		switch(child.GetKind())
		{
			case CXCursor_CXXBaseSpecifier:
			{
				auto baseClass = new BaseClass(child);

				m_BaseClasses.emplace_back(baseClass);

				//if don't find disable key, then m_Enabled don't be affected by this meta property
				if (isNativeType(baseClass->name))
					m_Enabled = !m_MetaData.GetFlag(nativeProperty::Disable);

				break;
			}
			//constructor
			case CXCursor_Constructor:
			{
				m_Constructors.emplace_back(new Constructor(child, currentNamespace, this));
				break;
			}
			case CXCursor_FieldDecl:
			{
				//construct the Field
				m_Fields.emplace_back(new Field(child, currentNamespace, this));
				
				break;
			}
			//method / static method
			case CXCursor_CXXMethod:
				if (child.IsStatic())
				{
					
				}
				else
				{
					m_Methods.emplace_back(new Method(child, currentNamespace, this));
				}
				break;
			default:
				break;
		}

	}

	for (size_t i = 0; i < m_Methods.size(); ++i)
	{
		for (size_t j = i + 1; j < m_Methods.size(); ++j)
		{
			if (m_Methods[i]->GetName() == m_Methods[j]->GetName())
			{
				for (size_t k = 0; k < m_Methods.size(); ++k)
				{
					if (m_Methods[i]->GetName() == m_Methods[k]->GetName())
						m_Methods[k]->SetOverload(true);
				}
				break;
			}
		}
	}

	//std::cout << "Field Size:";
	//std::cout << m_Fields.size() << std::endl;
}

bool Class::ShouldCompile() const
{
	//std::cout << isAccessible() << std::endl;
	//std::cout << !isNativeType(m_QualifiedName) << std::endl;
	return isAccessible() && !isNativeType(m_QualifiedName);
}

kainjow::mustache::data Class::CompileTemplate(const ReflectionParser* context) const
{
	kainjow::mustache::data data{ kainjow::mustache::data::type::object };

	data["displayName"] = m_DisplayName;

	data["qualifiedName"] = m_QualifiedName;

	//std::cout << "QualifiedName:" << m_QualifiedName << std::endl;

	data["ptrTypeEnabled"] = Utils::TemplateBool(m_ptrTypeEnabled);

	m_MetaData.CompileTemplateData(data, context);

	//base classes
	{
		kainjow::mustache::data baseClasses{ kainjow::mustache::data::type::list };

		int32_t i = 0;

		for (auto& baseClass : m_BaseClasses)
		{
			//ignore native types, MetaProperty and Object will be ignore
			if (isNativeType(baseClass->name))
				continue;;

			kainjow::mustache::data item{ kainjow::mustache::data::type::object };

			item["name"] = baseClass->name;

			item["isLast"] = Utils::TemplateBool(i == m_BaseClasses.size() - 1);

			baseClasses << item;

			++i;
		}

		data.set("baseClass", baseClasses);
	}

	//don't do anything else if only registering
	if (m_MetaData.GetFlag(nativeProperty::Register))
		return data;

	//constructors
	{
		kainjow::mustache::data constructors{ kainjow::mustache::data::type::list };

		for (auto& ctor : m_Constructors)
		{
			if (ctor->ShouldCompile())
				constructors << ctor->CompileTemplate(context);
		}

		data.set("constructor", constructors);
	}

	//fields
	{
		kainjow::mustache::data fields{ kainjow::mustache::data::type::list };

		for (auto& field : m_Fields)
		{
			if (field->ShouldCompile())
				fields << field->CompileTemplate(context);
		}

		data.set("field", fields);
	}

	//static methods
	{
		kainjow::mustache::data methods { kainjow::mustache::data::type::list };

		for (auto& method : m_Methods)
		{
			if (method->ShouldCompile())
				methods << method->CompileTemplate(context);
		}

		data.set("method", methods);
	}

	return data;
}

bool Class::isAccessible() const
{
	return m_Enabled || m_MetaData.GetFlag(nativeProperty::Register);
	//return true;//TODO:in the future, will give the meta data to determine the property
}
