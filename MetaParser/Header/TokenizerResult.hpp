#include "PreCompiled.h"

#include "Header/TokenizerResult.h"

template<typename TokenType>
inline TokenizerResult<TokenType>::TokenizerResult(const TokenList& tokens)
	:m_Tokens(tokens)
{
}

template<typename TokenType>
const typename TokenizerResult<TokenType>::TokenList& TokenizerResult<TokenType>::GetTokens() const
{
	return m_Tokens;
}

template<typename TokenType>
typename TokenType::InputValueType TokenizerResult<TokenType>::ConsumeRange(size_t start, size_t end)
{
	typename TokenType::InputValueType consumed{};

	end = std::min(end, m_Tokens.size() - 1);

	for (auto i = start; i <= end; ++i)
		consumed += m_Tokens[i].value;

	return consumed;
}

template<typename TokenType>
template<typename ...TokenTypeList>
inline size_t TokenizerResult<TokenType>::FindNext(size_t start, TokenTypeList && ...types)
{
	//enum vector
	std::vector<typename TokenType::EnumType> toMatch{ std::forward<TokenTypeList>(types)... };

	for (size_t i = start; i < m_Tokens.size(); ++i)
	{
		//from the toMatch to find corresponding enum type
		auto search = std::find(toMatch.begin(), toMatch.end(), m_Tokens[i].type);

		if (search != toMatch.end())
			return i;
	}

	return 0;
}

template<typename TokenType>
template<typename ...TokenTypeList>
size_t TokenizerResult<TokenType>::FindPrevious(size_t start, TokenTypeList && ...types)
{
	std::vector<typename TokenType::EnumType> toMatch{ std::forward<TokenTypeList>(types)... };

	for (auto i = 0; i <= start; ++i)
	{
		auto search = std::find(toMatch.begin(), toMatch.end(), m_Tokens[start - i].type);

		if (search != toMatch.end())
			return i;
	}

	return 0;
}

template<typename TokenType>
template<typename ...TokenTypeList>
void TokenizerResult<TokenType>::RemoveAll(TokenTypeList && ...types)
{
	std::vector<typename TokenType::EnumType> toRemove{ std::forward<TokenTypeList>(types)... };

	//from the m_Tokens to rid of the types
	//move to end
	auto removed = std::remove_if(
		m_Tokens.begin(),
		m_Tokens.end(),
		[&](const TokenType& token) {
		return std::find(toRemove.begin(), toRemove.end(), token.type) != toRemove.end();
	}
	);

	m_Tokens.erase(removed, m_Tokens.end());
}

template<typename TokenType>
template<typename ...TokenTypeList>
typename TokenType::InputValueType TokenizerResult<TokenType>::ConsumeAllPrevious(size_t start, TokenTypeList && ...types)
{
	std::vector<typename TokenType::EnumType> toConsume{ std::forward<TokenTypeList>(types)... };

	std::vector<typename TokenType::InputValueType*> consumed;

	for (size_t i = 0; i <= start; ++i)
	{
		auto& token = m_Tokens[start - i];

		auto search = std::find(toConsume.begin(), toConsume.end(), token.type);

		if(search == toConsume.end())
			break;

		consumed.emplace_back(&token.value);
	}

	typename TokenType::InputValueType result{};

	for (auto it = consumed.rbegin(); it != consumed.rend(); ++it)
		result += *(*it);

	return result;
}

template<typename TokenType>
template<typename ...TokenTypeList>
typename TokenType::InputValueType TokenizerResult<TokenType>::ConsumeAllNext(size_t start, TokenTypeList && ...types)
{
	std::vector<typename TokenType::EnumType> toConsume{ std::forward<TokenTypeList>(types)... };

	typename TokenType::InputValueType consumed{};

	for (size_t i = start; i < m_Tokens.size(); ++i)
	{
		auto& token = m_Tokens[start - i];

		auto search = std::find(toConsume.begin(), toConsume.end(), token.type);

		if (search == toConsume.end())
			break;

		consumed += token.value;
	}

	return consumed;
}
