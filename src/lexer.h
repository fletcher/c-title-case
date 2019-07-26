/**

	cTitleCase -- Apply title case rules.

	@file lexer.h

	@brief


	@author	Fletcher T. Penney
	@bug

**/

/*

	Copyright © 2017 Fletcher T. Penney.


	The `c-template` project is released under the MIT License.

	GLibFacade.c and GLibFacade.h are from the MultiMarkdown v4 project:

		https://github.com/fletcher/MultiMarkdown-4/

	MMD 4 is released under both the MIT License and GPL.


	CuTest is released under the zlib/libpng license. See CuTest.c for the text
	of the license.


	## The MIT License ##

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

*/


#ifndef LEXER_C_TEMPLATE_H
#define LEXER_C_TEMPLATE_H

#ifdef TEST
	#include "CuTest.h"
#endif

enum word_types {
	WORD_SHORT = 1,
	WORD_UPPER,
	WORD_MIXED,
	WORD_PLAIN,
	WORD_LAST,
	START_SUBSENTENCE,
	START_SUBPHRASE,
	END_SUBPHRASE,
	PUNCT,
	WORD_URL,
	WORD_UTF8
};




/// Re2c scanner data -- this structure is used by the re2c
/// lexer to track progress and offsets within the source
/// string.  They can be used to create "tokens" that match
/// sections of the text with an abstract syntax tree.
struct Scanner {
	const char *	start;		//!< Start of current token
	const char *	cur;		//!< Character currently being matched
	const char *	ptr;		//!< Used for backtracking by re2c
	const char *	ctx;
};

typedef struct Scanner Scanner;


#endif
