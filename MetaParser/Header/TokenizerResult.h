#pragma once

template<typename TokenType>
class TokenizerResult
{
public:
	using TokenList = std::vector<TokenType>;

	TokenizerResult(const TokenList& tokens);

	const TokenList& GetTokens() const;

	//to search whether or not the [m_Tokens + start, m_Tokens.end()] match types
	//return don't equal place
	template<typename... TokenTypeList>
	size_t FindNext(size_t start, TokenTypeList&&... types);

	template<typename... TokenTypeList>
	size_t FindPrevious(size_t start, TokenTypeList&&... types);

	template<typename... TokenTypeList>
	void RemoveAll(TokenTypeList&&... types);

	//connect the [start, end] string
	typename TokenType::InputValueType ConsumeRange(size_t start, size_t end);

	template<typename... TokenTypeList>
	typename TokenType::InputValueType ConsumeAllPrevious(size_t start, TokenTypeList&&... types);

	template<typename... TokenTypeList>
	typename TokenType::InputValueType ConsumeAllNext(size_t start, TokenTypeList&&... types);
private:
	TokenList m_Tokens;
};

#include "TokenizerResult.hpp"

