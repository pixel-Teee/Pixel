#include "PreCompiled.h"

#include "Header/Tokenizer.h"

template<typename TokenEnumType, typename InputType>
inline Tokenizer<TokenEnumType, InputType>::Tokenizer()
	:m_RootState(CreateState())
{
}

template<typename TokenEnumType, typename InputType>
Tokenizer<TokenEnumType, InputType>::~Tokenizer()
{
	for (auto* state : m_CreatedStates)
		delete state;
}

template<typename TokenEnumType, typename InputType>
const typename Tokenizer<TokenEnumType, InputType>::StateType::Handle Tokenizer<TokenEnumType, InputType>::GetRootState() const
{
	return m_RootState;
}

template<typename TokenEnumType, typename InputType>
typename Tokenizer<TokenEnumType, InputType>::StateType::Handle Tokenizer<TokenEnumType, InputType>::CreateState(TokenEnumType acceptingType)
{
	auto state = new StateType(acceptingType);//create a TokenizerState

	m_CreatedStates.push_back(state);

	return state;
}

template<typename TokenEnumType, typename InputType>
void Tokenizer<TokenEnumType, InputType>::LoadSymbols(const SymbolTable& symbols)
{
	for (auto& symbol : symbols)
	{
		auto currentState = m_RootState;

		auto& value = symbol.first;

		auto length = value.size();

		for (size_t i = 0; i < length; ++i)
		{
			auto character = value[i];

			typename StateType::Handle nextState;

			auto isLastCharacter = i == length - 1;//last character

			auto search = currentState->m_Edges.find(character);
			
			//don't exist, then create a new state
			if (search == currentState->m_Edges.end())
			{
				nextState = CreateState();

				currentState->AddEdge(nextState, character);
			}
			else
			{
				nextState = search->second;
			}

			if (isLastCharacter)
				nextState->m_AcceptingType = symbol.second;

			currentState = nextState;
		}
	}
}

template<typename TokenEnumType, typename InputType>
typename Tokenizer<TokenEnumType, InputType>::ResultType Tokenizer<TokenEnumType, InputType>::Tokenize(const InputType& input) const
{
	typename ResultType::TokenList tokens;

	auto iterator = input.begin();

	//from the string to find some tokens, in terms of the Tokenzier's symbol table
	while (iterator < input.end())
	{
		TokenType token;

		//from the string to find a token
		ReadToken(iterator, input.end(), token);

		//to read next token
		iterator += token.value.size();

		if (token.value.empty())
			++iterator;
		else
			tokens.push_back(token);
	}

	return ResultType(tokens);
}

template<typename TokenEnumType, typename InputType>
void Tokenizer<TokenEnumType, InputType>::ReadToken(InputIterator start, InputIterator end, TokenType& outputToken) const
{
	//from the std::string iterator [start, end], to read token
	outputToken.type = TokenEnumType::Invalid;

	auto accepted = start;
	auto lastAccepted = start;

	auto currentState = m_RootState;

	auto iterator = start;

	for (; iterator != end; ++iterator)
	{
		auto transition = currentState->m_Edges.find(*iterator);
		auto transitionFound = transition != currentState->m_Edges.end();

		auto nextState = transitionFound ? transition->second : currentState->m_DefaultEdge;

		//terminating state
		if (!nextState)
		{
			accepted = (lastAccepted == start) ? iterator : lastAccepted;

			break;
		}

		if (nextState->m_AcceptingType != TokenEnumType::Invalid)
		{
			outputToken.type = nextState->m_AcceptingType;

			lastAccepted = iterator + 1;
		}

		currentState = nextState;
	}

	//we read until the end of input
	if (iterator == end)
		accepted = end;

	outputToken.value = InputType(start, accepted);
}
