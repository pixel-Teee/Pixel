#include "PreCompiled.h"

#include "Header/MetaDataManager.h"

#include "Header/TokenType.h"
#include "Header/Tokenizer.h"

#include "Header/MetaDataConfig.h"
#include "Header/MetaUtils.h"

#define TOKENS_WHITESPACE ' ', '\r', '\n', '\t'

#define TOKENS_NUMBER       '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
#define TOKENS_ALPHANUMERIC 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', \
                            'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', \
                            's', 't', 'u', 'v', 'w', 'x', 'y', 'z',      \
                            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', \
                            'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', \
                            'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'       \

#define LITERAL_ESCAPE_CHAR '\\'
#define LITERAL_SINGLE_QUOTE '\''
#define LITERAL_DOUBLE_QUOTE '"'

//effect getConstructorTokenizer
#define TOKEN(name, value) {value, ConstructorTokenType::name},


	//using ConstructorToken = Token<ConstructorTokenType, std::string>;
	//
	//const Tokenizer<ConstructorTokenType>

typedef Token<ConstructorTokenType, std::string> ConstructorToken;

const Tokenizer<ConstructorTokenType>& getConstructorTokenizer();


MetaDataManager::MetaDataManager(const Cursor& cursor)
{
	//get class field function's annotate

	//"Key = Value, Key2, Key = "Yep!""
	for (auto& child : cursor.GetChildren())
	{
		if(child.GetKind() != CXCursor_AnnotateAttr)
			continue;

		m_Properties["HaveAnnotate"] = "";

		for (auto& prop : extractProperties(child))
		{
			m_Properties[prop.first] = prop.second;
			//std::cout << "Property Key:" << prop.first << std::endl;
			//std::cout << "Property Value:" << prop.second << std::endl;
			std::cout << "Property Key:" << prop.first << std::endl;
			std::cout << "Property Value:" << prop.second << std::endl;
		}
	}
}

std::string MetaDataManager::GetProperty(const std::string& key) const
{
	auto search = m_Properties.find(key);

	//use an empty string by default

	return search == m_Properties.end() ? "" : search->second;
}

bool MetaDataManager::GetFlag(const std::string& key) const
{
	return m_Properties.find(key) == m_Properties.end() ? false : true;
}

std::string MetaDataManager::GetNativeString(const std::string& key) const
{
	auto search = m_Properties.find(key);

	//wasn't set
	if (search == m_Properties.end())
		return "";

	//quoted
	static const std::regex qutotedString(
		//opening quote
		"(?:\\s*\")"

		//actual string contents
		"([^\"]*)"

		//closing quote
		"\"",
		std::regex::icase
	);

	auto& value = search->second;

	auto flags = std::regex_constants::match_default;

	//from the "xx" to get the xx
	std::smatch match;

	if (std::regex_search(value.cbegin(), value.cend(), match, qutotedString, flags))
	{
		//get actual string content
		return match[1].str();
	}

	//couldn't find one
	return "";
}

void MetaDataManager::CompileTemplateData(kainjow::mustache::data& data, const ReflectionParser* context) const
{
	kainjow::mustache::data propertyData{ kainjow::mustache::data::type::list };

	std::vector<std::string> lines;

	//list of keywords to ignore in the initializer list
	static const std::vector<std::string> reservedKeywords
	{
		nativeProperty::HaveAnnotate,
		nativeProperty::Enable,
		nativeProperty::Disable,
		nativeProperty::Register,
		nativeProperty::WhiteListMethods,
		nativeProperty::DisableNonDynamicCtor,
		nativeProperty::DynamicCtorWrap,
		nativeProperty::EnablePtrType,
		nativeProperty::EnableConstPtrType,
		nativeProperty::EnableArrayType,
		nativeProperty::DisplayName,
		nativeProperty::ExplicitGetter,
		nativeProperty::ExplicitSetter,
		nativeProperty::VeryExplicitGetter,
		nativeProperty::VeryExplicitSetter
	};

	int32_t i = 0;

	auto propertyCount = m_Properties.size() - 1;

	for (auto& prop : m_Properties)
	{
		kainjow::mustache::data item{ kainjow::mustache::data::type::object };

		//skip reserved keywords
		if (std::find(reservedKeywords.begin(), reservedKeywords.end(), prop.first) != reservedKeywords.end())
		{
			--propertyCount;

			continue;
		}

		item["type"] = prop.first;
		item["arguments"] = prop.second;
		item["isLast"] = Utils::TemplateBool(i == propertyCount);

		propertyData << item;

		++i;
	}

	if (m_Properties.find("HaveAnnotate") != m_Properties.end())
		data.set("HaveAnnotate", true);
	else
		data.set("HaveAnnotate", false);

	if (i == 0)
		data.set("metaPropertyHaveBrackets", false);
	else
		data.set("metaPropertyHaveBrackets", true);
	data.set("metaProperty", propertyData);
	//data["metaProperty"] = propertyData;
	//data["metaDataInitializerList"];
}

std::vector<MetaDataManager::Property> MetaDataManager::extractProperties(const Cursor& cursor) const
{
	//std::vector<std::pair<std::string, std::string>>
	std::vector<Property> properties;

	auto& tokenizer = getConstructorTokenizer();

	auto propertyList = cursor.GetDisplayName();

	//std::cout << "property list:";
	//std::cout << propertyList;

	//to extract the property
	auto result = tokenizer.Tokenize(propertyList);

	result.RemoveAll(ConstructorTokenType::Whitespace);

	auto& tokens = result.GetTokens();
	int32_t tokenCount = static_cast<int32_t>(tokens.size());

	//case where there is only one identifier, which means there's one property with a default constructor
	if (tokenCount == 1 && tokens[0].type == ConstructorTokenType::Identifier)
	{
		properties.emplace_back(tokens[0].value, "");
	}

	auto lastType = ConstructorTokenType::Invalid;

	int32_t firstOpenParenToken = 0;
	int32_t openParens = 0;

	//identifier::<constructor content>
	for (int32_t i = 0; i < tokenCount; ++i)
	{
		auto& token = tokens[i];

		switch (token.type)
		{
		case ConstructorTokenType::OpenParentheses://'('
		{
			if (openParens == 0)
				firstOpenParenToken = i;

			++openParens;

			break;
		}
		case ConstructorTokenType::CloseParentheses:
		{
			--openParens;

			//we have read a constructor
			if (openParens == 0)
			{
				properties.emplace_back(
					result.ConsumeAllPrevious(std::max(0, firstOpenParenToken - 1),
						ConstructorTokenType::Identifier,
						ConstructorTokenType::ScopeResolution,
						ConstructorTokenType::LessThan,
						ConstructorTokenType::GreaterThan
					),
					result.ConsumeRange(firstOpenParenToken + 1, std::max(0, i - 1))//identifier::<> and constructor parameter content
				);
				++i;
			}
			break;
		}
		case ConstructorTokenType::Comma:
		{
			//constructor with no parameters
			if (openParens == 0 && lastType == ConstructorTokenType::Identifier)
			{
				properties.emplace_back(
					result.ConsumeAllPrevious(i - 1,
						ConstructorTokenType::Identifier,
						ConstructorTokenType::ScopeResolution,
						ConstructorTokenType::LessThan,
						ConstructorTokenType::GreaterThan
					),
					""
				);
			}

			break;
		}
		default:
			break;
		}

		lastType = token.type;
	}

	//case where a default constructor is the last in the list
	if (tokenCount >= 2 &&
		tokens[tokenCount - 1].type == ConstructorTokenType::Identifier &&
		tokens[tokenCount - 2].type == ConstructorTokenType::Comma)
	{
		properties.emplace_back(tokens.back().value, "");
	}

	return properties;
}

const Tokenizer<ConstructorTokenType>& getConstructorTokenizer()
{
	{

		static bool initialized = false;
		static Tokenizer<ConstructorTokenType> tokenizer;

		//std::cout << "Terminal1" << std::endl;

		if (initialized)
			return tokenizer;

		//std::cout << "Terminal2" << std::endl;

		auto root = tokenizer.GetRootState();

		//white space
		{
			//create a tokenizer state
			auto whiteSpace = tokenizer.CreateState(ConstructorTokenType::Whitespace);

			//add edge
			whiteSpace->SetLooping(TOKENS_WHITESPACE);

			root->AddEdge(whiteSpace, TOKENS_WHITESPACE);//this error
		}

		//std::cout << "Terminal3" << std::endl;

		//identifier
		{
			auto firstCharacter = tokenizer.CreateState(ConstructorTokenType::Identifier);
			auto anyCharacters = tokenizer.CreateState(ConstructorTokenType::Identifier);

			root->AddEdge(firstCharacter, TOKENS_ALPHANUMERIC, '_');//this error

			anyCharacters->SetLooping(TOKENS_ALPHANUMERIC, TOKENS_NUMBER, '_');

			firstCharacter->AddEdge(anyCharacters, TOKENS_ALPHANUMERIC, TOKENS_NUMBER, '_');
		}		

		//integer literal
		auto integerLiteral = tokenizer.CreateState(ConstructorTokenType::IntegerLiteral);
		{
			integerLiteral->SetLooping(TOKENS_NUMBER);

			root->AddEdge(integerLiteral, TOKENS_NUMBER);
		}

		//float literal
		{
			auto period = tokenizer.CreateState();
			auto floatNoExponent = tokenizer.CreateState(ConstructorTokenType::FloatLiteral);
			auto exponent = tokenizer.CreateState();
			auto plusOrMinus = tokenizer.CreateState();
			auto validOptionalExponent = tokenizer.CreateState(ConstructorTokenType::FloatLiteral);
			auto fCharacter = tokenizer.CreateState(ConstructorTokenType::FloatLiteral);

			integerLiteral->AddEdge(period, '.');
			period->AddEdge(floatNoExponent, TOKENS_NUMBER);

			floatNoExponent->AddEdge(exponent, 'e', 'E');
			floatNoExponent->SetLooping(TOKENS_NUMBER);

			exponent->AddEdge(validOptionalExponent, TOKENS_NUMBER);
			exponent->AddEdge(plusOrMinus, '+', '-');

			plusOrMinus->AddEdge(validOptionalExponent, TOKENS_NUMBER);

			validOptionalExponent->AddEdge(fCharacter, 'f', 'F');
			validOptionalExponent->SetLooping(TOKENS_NUMBER);

			floatNoExponent->AddEdge(fCharacter, 'f', 'F');
		}

		//string literal
		{
			auto firstDoubleQuote = tokenizer.CreateState();
			auto escapeSlash = tokenizer.CreateState();
			auto escapeChars = tokenizer.CreateState();
			auto validStringLiteral = tokenizer.CreateState(ConstructorTokenType::StringLiteral);
			auto anyCharacter = tokenizer.CreateState();

			root->AddEdge(firstDoubleQuote, LITERAL_DOUBLE_QUOTE);

			firstDoubleQuote->AddEdge(escapeSlash, LITERAL_ESCAPE_CHAR);
			firstDoubleQuote->AddEdge(validStringLiteral, LITERAL_DOUBLE_QUOTE);
			firstDoubleQuote->SetDefaultEdge(anyCharacter);

			escapeSlash->AddEdge(escapeChars, LITERAL_DOUBLE_QUOTE);

			// accept any escape token
			escapeSlash->SetDefaultEdge(firstDoubleQuote);

			escapeChars->SetDefaultEdge(firstDoubleQuote);
			escapeChars->AddEdge(validStringLiteral, LITERAL_DOUBLE_QUOTE);

			anyCharacter->SetDefaultEdge(anyCharacter);
			anyCharacter->AddEdge(escapeSlash, LITERAL_ESCAPE_CHAR);
			anyCharacter->AddEdge(validStringLiteral, LITERAL_DOUBLE_QUOTE);
		}

		//symbols
		{
			decltype(tokenizer)::SymbolTable symbols
			{
				#include "Header/ConstructorTokenSymbols.inl"
			};

			tokenizer.LoadSymbols(symbols);
		}

		//std::cout << "Ternimal" << std::endl;

		return tokenizer;
	}
}




