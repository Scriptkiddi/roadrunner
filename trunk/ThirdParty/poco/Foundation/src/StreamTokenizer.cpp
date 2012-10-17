//
// StreamTokenizer.cpp
//
// $Id: //poco/1.4/Foundation/src/StreamTokenizer.cpp#1 $
//
// Library: Foundation
// Package: Streams
// Module:  StreamTokenizer
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/StreamTokenizer.h"


namespace Poco {


StreamTokenizer::StreamTokenizer():
	_pIstr(0)
{
}


StreamTokenizer::StreamTokenizer(std::istream& istr):
	_pIstr(&istr)
{
}


StreamTokenizer::~StreamTokenizer()
{
	for (TokenVec::iterator it = _tokens.begin(); it != _tokens.end(); ++it)
	{
		delete it->pToken;
	}
}


void StreamTokenizer::attachToStream(std::istream& istr)
{
	_pIstr = &istr;
}


void StreamTokenizer::addToken(Token* pToken)
{
	poco_check_ptr (pToken);

	TokenInfo ti;
	ti.pToken = pToken;
	ti.ignore = (pToken->tokenClass() == Token::COMMENT_TOKEN || pToken->tokenClass() == Token::WHITESPACE_TOKEN);
	_tokens.push_back(ti);
}


void StreamTokenizer::addToken(Token* pToken, bool ignore)
{
	poco_check_ptr (pToken);

	TokenInfo ti;
	ti.pToken = pToken;
	ti.ignore = ignore;
	_tokens.push_back(ti);
}

	
const Token* StreamTokenizer::next()
{
	poco_check_ptr (_pIstr);
	
	static const int eof = std::char_traits<char>::eof();

	int first = _pIstr->get();
	TokenVec::const_iterator it = _tokens.begin();
	while (first != eof && it != _tokens.end())
	{
		const TokenInfo& ti = *it;
		if (ti.pToken->start((char) first, *_pIstr))
		{
			ti.pToken->finish(*_pIstr);
			if (ti.ignore) 
			{
				first = _pIstr->get();
				it = _tokens.begin();
			}
			else return ti.pToken;
		}
		else ++it;
	}
	if (first == eof)
	{
		return &_eofToken;
	}
	else
	{
		_invalidToken.start((char) first, *_pIstr);
		return &_invalidToken;
	}
}


} // namespace Poco
