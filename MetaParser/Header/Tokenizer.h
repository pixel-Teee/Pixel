#pragma once

#include "Token.h"
#include "TokenizerState.h"
#include "TokenizerResult.h"

//automation
template<typename TokenEnumType, typename InputType = std::string>
class Tokenizer
{
public:
	using TokenType = Token<TokenEnumType, InputType>;

	using ResultType = TokenizerResult<TokenType>;

	using StateType = TokenizerState<TokenEnumType, InputType>;

	using InputIterator = typename InputType::const_iterator;

	using SymbolTable = std::unordered_map<InputType, TokenEnumType>;
	//using ResultType = TokenizerResult<TokenType>;

	Tokenizer();

	~Tokenizer();

	const typename StateType::Handle GetRootState() const;

	//create TokenizerState to push to m_CreatedStates
	typename StateType::Handle CreateState(TokenEnumType acceptingType = TokenEnumType::Invalid);

	void LoadSymbols(const SymbolTable& table);

	//from input std::string to read some tokens push to ResultType
	ResultType Tokenize(const InputType& input) const;
private:

	void ReadToken(InputIterator start, InputIterator end, TokenType& outputToken) const;

	std::vector<typename StateType::Handle> m_CreatedStates;

	//TokenizerState*
	typename StateType::Handle m_RootState;
};

#include "Tokenizer.hpp"


