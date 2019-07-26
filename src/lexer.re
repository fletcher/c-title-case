/**

	cTitleCase -- Apply title case rules.

	@file lexer.re

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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "char.h"
#include "lexer.h"
#include "libTitleCase.h"

#include "d_string.h"

#include "utf8proc.h"


// Basic scanner struct

#define YYCTYPE			unsigned char
#define YYCURSOR		s->cur
#define YYMARKER		s->ptr
#define YYCTXMARKER		s->ctx


#define print_cap		d_string_append_c(out, toupper(*(s.start))); for (size_t i = (size_t)(s.start + 1 - in->str); i < (s.cur - in->str); i++) { d_string_append_c(out, tolower(str[i])); }
#define print_lower		d_string_append_c(out, tolower(*(s.start))); d_string_append_c_array(out, s.start + 1, (int)(s.cur - s.start - 1));
#define print_as_is		d_string_append_c_array(out, s.start, (int)(s.cur - s.start));
#define print_upper		upper = true; for (size_t i = (size_t)(s.start - in->str); i < (s.cur - in->str); i++) { if (str[i] == '\'') {upper = false; } if (upper) {d_string_append_c(out, toupper(str[i]));} else {d_string_append_c(out, str[i]); } }


static int scan(Scanner * s, const char * stop) {

scan:

	if (s->cur >= stop) {
		return 0;
	}

	s->start = s->cur;

	/*!re2c
		re2c:yyfill:enable = 0;

		shortwords	= ( 'a' | 'an' | 'and' | 'as' | 'at' | 'but' | 'by' | 'en' |
			'for' | 'if' | 'in' | 'of' | 'on' | 'or' | 'the' | 'to' | 'v' |
			'v.' | 'via' | 'vs' | 'vs.' );

		// Unicode letter group without CJK (I think)
		// https://github.com/skvadrik/re2c/issues/235
		NON_ASCII = [\xaa-\xaa\xb5-\xb5\xba-\xba\xc0-\xd6\xd8-\xf6\xf8-\u02c1\u02c6-\u02d1\u02e0-\u02e4\u02ec-\u02ec\u02ee-\u02ee\u0370-\u0374\u0376-\u0377\u037a-\u037d\u037f-\u037f\u0386-\u0386\u0388-\u038a\u038c-\u038c\u038e-\u03a1\u03a3-\u03f5\u03f7-\u0481\u048a-\u052f\u0531-\u0556\u0559-\u0559\u0561-\u0587\u05d0-\u05ea\u05f0-\u05f2\u0620-\u064a\u066e-\u066f\u0671-\u06d3\u06d5-\u06d5\u06e5-\u06e6\u06ee-\u06ef\u06fa-\u06fc\u06ff-\u06ff\u0710-\u0710\u0712-\u072f\u074d-\u07a5\u07b1-\u07b1\u07ca-\u07ea\u07f4-\u07f5\u07fa-\u07fa\u0800-\u0815\u081a-\u081a\u0824-\u0824\u0828-\u0828\u0840-\u0858\u08a0-\u08b2\u0904-\u0939\u093d-\u093d\u0950-\u0950\u0958-\u0961\u0971-\u0980\u0985-\u098c\u098f-\u0990\u0993-\u09a8\u09aa-\u09b0\u09b2-\u09b2\u09b6-\u09b9\u09bd-\u09bd\u09ce-\u09ce\u09dc-\u09dd\u09df-\u09e1\u09f0-\u09f1\u0a05-\u0a0a\u0a0f-\u0a10\u0a13-\u0a28\u0a2a-\u0a30\u0a32-\u0a33\u0a35-\u0a36\u0a38-\u0a39\u0a59-\u0a5c\u0a5e-\u0a5e\u0a72-\u0a74\u0a85-\u0a8d\u0a8f-\u0a91\u0a93-\u0aa8\u0aaa-\u0ab0\u0ab2-\u0ab3\u0ab5-\u0ab9\u0abd-\u0abd\u0ad0-\u0ad0\u0ae0-\u0ae1\u0b05-\u0b0c\u0b0f-\u0b10\u0b13-\u0b28\u0b2a-\u0b30\u0b32-\u0b33\u0b35-\u0b39\u0b3d-\u0b3d\u0b5c-\u0b5d\u0b5f-\u0b61\u0b71-\u0b71\u0b83-\u0b83\u0b85-\u0b8a\u0b8e-\u0b90\u0b92-\u0b95\u0b99-\u0b9a\u0b9c-\u0b9c\u0b9e-\u0b9f\u0ba3-\u0ba4\u0ba8-\u0baa\u0bae-\u0bb9\u0bd0-\u0bd0\u0c05-\u0c0c\u0c0e-\u0c10\u0c12-\u0c28\u0c2a-\u0c39\u0c3d-\u0c3d\u0c58-\u0c59\u0c60-\u0c61\u0c85-\u0c8c\u0c8e-\u0c90\u0c92-\u0ca8\u0caa-\u0cb3\u0cb5-\u0cb9\u0cbd-\u0cbd\u0cde-\u0cde\u0ce0-\u0ce1\u0cf1-\u0cf2\u0d05-\u0d0c\u0d0e-\u0d10\u0d12-\u0d3a\u0d3d-\u0d3d\u0d4e-\u0d4e\u0d60-\u0d61\u0d7a-\u0d7f\u0d85-\u0d96\u0d9a-\u0db1\u0db3-\u0dbb\u0dbd-\u0dbd\u0dc0-\u0dc6\u0e01-\u0e30\u0e32-\u0e33\u0e40-\u0e46\u0e81-\u0e82\u0e84-\u0e84\u0e87-\u0e88\u0e8a-\u0e8a\u0e8d-\u0e8d\u0e94-\u0e97\u0e99-\u0e9f\u0ea1-\u0ea3\u0ea5-\u0ea5\u0ea7-\u0ea7\u0eaa-\u0eab\u0ead-\u0eb0\u0eb2-\u0eb3\u0ebd-\u0ebd\u0ec0-\u0ec4\u0ec6-\u0ec6\u0edc-\u0edf\u0f00-\u0f00\u0f40-\u0f47\u0f49-\u0f6c\u0f88-\u0f8c\u1000-\u102a\u103f-\u103f\u1050-\u1055\u105a-\u105d\u1061-\u1061\u1065-\u1066\u106e-\u1070\u1075-\u1081\u108e-\u108e\u10a0-\u10c5\u10c7-\u10c7\u10cd-\u10cd\u10d0-\u10fa\u10fc-\u1248\u124a-\u124d\u1250-\u1256\u1258-\u1258\u125a-\u125d\u1260-\u1288\u128a-\u128d\u1290-\u12b0\u12b2-\u12b5\u12b8-\u12be\u12c0-\u12c0\u12c2-\u12c5\u12c8-\u12d6\u12d8-\u1310\u1312-\u1315\u1318-\u135a\u1380-\u138f\u13a0-\u13f4\u1401-\u166c\u166f-\u167f\u1681-\u169a\u16a0-\u16ea\u16f1-\u16f8\u1700-\u170c\u170e-\u1711\u1720-\u1731\u1740-\u1751\u1760-\u176c\u176e-\u1770\u1780-\u17b3\u17d7-\u17d7\u17dc-\u17dc\u1820-\u1877\u1880-\u18a8\u18aa-\u18aa\u18b0-\u18f5\u1900-\u191e\u1950-\u196d\u1970-\u1974\u1980-\u19ab\u19c1-\u19c7\u1a00-\u1a16\u1a20-\u1a54\u1aa7-\u1aa7\u1b05-\u1b33\u1b45-\u1b4b\u1b83-\u1ba0\u1bae-\u1baf\u1bba-\u1be5\u1c00-\u1c23\u1c4d-\u1c4f\u1c5a-\u1c7d\u1ce9-\u1cec\u1cee-\u1cf1\u1cf5-\u1cf6\u1d00-\u1dbf\u1e00-\u1f15\u1f18-\u1f1d\u1f20-\u1f45\u1f48-\u1f4d\u1f50-\u1f57\u1f59-\u1f59\u1f5b-\u1f5b\u1f5d-\u1f5d\u1f5f-\u1f7d\u1f80-\u1fb4\u1fb6-\u1fbc\u1fbe-\u1fbe\u1fc2-\u1fc4\u1fc6-\u1fcc\u1fd0-\u1fd3\u1fd6-\u1fdb\u1fe0-\u1fec\u1ff2-\u1ff4\u1ff6-\u1ffc\u2071-\u2071\u207f-\u207f\u2090-\u209c\u2102-\u2102\u2107-\u2107\u210a-\u2113\u2115-\u2115\u2119-\u211d\u2124-\u2124\u2126-\u2126\u2128-\u2128\u212a-\u212d\u212f-\u2139\u213c-\u213f\u2145-\u2149\u214e-\u214e\u2183-\u2184\u2c00-\u2c2e\u2c30-\u2c5e\u2c60-\u2ce4\u2ceb-\u2cee\u2cf2-\u2cf3\u2d00-\u2d25\u2d27-\u2d27\u2d2d-\u2d2d\u2d30-\u2d67\u2d6f-\u2d6f\u2d80-\u2d96\u2da0-\u2da6\u2da8-\u2dae\u2db0-\u2db6\u2db8-\u2dbe\u2dc0-\u2dc6\u2dc8-\u2dce\u2dd0-\u2dd6\u2dd8-\u2dde\u2e2f-\u2e2f\ua000-\ua48c\ua4d0-\ua4fd\ua500-\ua60c\ua610-\ua61f\ua62a-\ua62b\ua640-\ua66e\ua67f-\ua69d\ua6a0-\ua6e5\ua722-\ua788\ua78b-\ua78e\ua790-\ua7ad\ua7b0-\ua7b1\ua7f7-\ua801\ua803-\ua805\ua807-\ua80a\ua80c-\ua822\ua840-\ua873\ua882-\ua8b3\ua8f2-\ua8f7\ua8fb-\ua8fb\ua90a-\ua925\ua930-\ua946\ua960-\ua97c\ua984-\ua9b2\ua9cf-\ua9cf\ua9e0-\ua9e4\ua9e6-\ua9ef\ua9fa-\ua9fe\uaa00-\uaa28\uaa40-\uaa42\uaa44-\uaa4b\uaa60-\uaa76\uaa7a-\uaa7a\uaa7e-\uaaaf\uaab1-\uaab1\uaab5-\uaab6\uaab9-\uaabd\uaac0-\uaac0\uaac2-\uaac2\uaadb-\uaadd\uaae0-\uaaea\uaaf2-\uaaf4\uab01-\uab06\uab09-\uab0e\uab11-\uab16\uab20-\uab26\uab28-\uab2e\uab30-\uab5a\uab5c-\uab5f\uab64-\uab65\uabc0-\uabe2\ud7b0-\ud7c6\ud7cb-\ud7fb\ufb00-\ufb06\ufb13-\ufb17\ufb1d-\ufb1d\ufb1f-\ufb28\ufb2a-\ufb36\ufb38-\ufb3c\ufb3e-\ufb3e\ufb40-\ufb41\ufb43-\ufb44\ufb46-\ufbb1\ufbd3-\ufd3d\ufd50-\ufd8f\ufd92-\ufdc7\ufdf0-\ufdfb\ufe70-\ufe74\ufe76-\ufefc\U00010000-\U0001000b\U0001000d-\U00010026\U00010028-\U0001003a\U0001003c-\U0001003d\U0001003f-\U0001004d\U00010050-\U0001005d\U00010080-\U000100fa\U00010280-\U0001029c\U000102a0-\U000102d0\U00010300-\U0001031f\U00010330-\U00010340\U00010342-\U00010349\U00010350-\U00010375\U00010380-\U0001039d\U000103a0-\U000103c3\U000103c8-\U000103cf\U00010400-\U0001049d\U00010500-\U00010527\U00010530-\U00010563\U00010600-\U00010736\U00010740-\U00010755\U00010760-\U00010767\U00010800-\U00010805\U00010808-\U00010808\U0001080a-\U00010835\U00010837-\U00010838\U0001083c-\U0001083c\U0001083f-\U00010855\U00010860-\U00010876\U00010880-\U0001089e\U00010900-\U00010915\U00010920-\U00010939\U00010980-\U000109b7\U000109be-\U000109bf\U00010a00-\U00010a00\U00010a10-\U00010a13\U00010a15-\U00010a17\U00010a19-\U00010a33\U00010a60-\U00010a7c\U00010a80-\U00010a9c\U00010ac0-\U00010ac7\U00010ac9-\U00010ae4\U00010b00-\U00010b35\U00010b40-\U00010b55\U00010b60-\U00010b72\U00010b80-\U00010b91\U00010c00-\U00010c48\U00011003-\U00011037\U00011083-\U000110af\U000110d0-\U000110e8\U00011103-\U00011126\U00011150-\U00011172\U00011176-\U00011176\U00011183-\U000111b2\U000111c1-\U000111c4\U000111da-\U000111da\U00011200-\U00011211\U00011213-\U0001122b\U000112b0-\U000112de\U00011305-\U0001130c\U0001130f-\U00011310\U00011313-\U00011328\U0001132a-\U00011330\U00011332-\U00011333\U00011335-\U00011339\U0001133d-\U0001133d\U0001135d-\U00011361\U00011480-\U000114af\U000114c4-\U000114c5\U000114c7-\U000114c7\U00011580-\U000115ae\U00011600-\U0001162f\U00011644-\U00011644\U00011680-\U000116aa\U000118a0-\U000118df\U000118ff-\U000118ff\U00011ac0-\U00011af8\U00012000-\U00012398\U00013000-\U0001342e\U00016800-\U00016a38\U00016a40-\U00016a5e\U00016ad0-\U00016aed\U00016b00-\U00016b2f\U00016b40-\U00016b43\U00016b63-\U00016b77\U00016b7d-\U00016b8f\U00016f00-\U00016f44\U00016f50-\U00016f50\U00016f93-\U00016f9f\U0001b000-\U0001b001\U0001bc00-\U0001bc6a\U0001bc70-\U0001bc7c\U0001bc80-\U0001bc88\U0001bc90-\U0001bc99\U0001d400-\U0001d454\U0001d456-\U0001d49c\U0001d49e-\U0001d49f\U0001d4a2-\U0001d4a2\U0001d4a5-\U0001d4a6\U0001d4a9-\U0001d4ac\U0001d4ae-\U0001d4b9\U0001d4bb-\U0001d4bb\U0001d4bd-\U0001d4c3\U0001d4c5-\U0001d505\U0001d507-\U0001d50a\U0001d50d-\U0001d514\U0001d516-\U0001d51c\U0001d51e-\U0001d539\U0001d53b-\U0001d53e\U0001d540-\U0001d544\U0001d546-\U0001d546\U0001d54a-\U0001d550\U0001d552-\U0001d6a5\U0001d6a8-\U0001d6c0\U0001d6c2-\U0001d6da\U0001d6dc-\U0001d6fa\U0001d6fc-\U0001d714\U0001d716-\U0001d734\U0001d736-\U0001d74e\U0001d750-\U0001d76e\U0001d770-\U0001d788\U0001d78a-\U0001d7a8\U0001d7aa-\U0001d7c2\U0001d7c4-\U0001d7cb\U0001e800-\U0001e8c4\U0001ee00-\U0001ee03\U0001ee05-\U0001ee1f\U0001ee21-\U0001ee22\U0001ee24-\U0001ee24\U0001ee27-\U0001ee27\U0001ee29-\U0001ee32\U0001ee34-\U0001ee37\U0001ee39-\U0001ee39\U0001ee3b-\U0001ee3b\U0001ee42-\U0001ee42\U0001ee47-\U0001ee47\U0001ee49-\U0001ee49\U0001ee4b-\U0001ee4b\U0001ee4d-\U0001ee4f\U0001ee51-\U0001ee52\U0001ee54-\U0001ee54\U0001ee57-\U0001ee57\U0001ee59-\U0001ee59\U0001ee5b-\U0001ee5b\U0001ee5d-\U0001ee5d\U0001ee5f-\U0001ee5f\U0001ee61-\U0001ee62\U0001ee64-\U0001ee64\U0001ee67-\U0001ee6a\U0001ee6c-\U0001ee72\U0001ee74-\U0001ee77\U0001ee79-\U0001ee7c\U0001ee7e-\U0001ee7e\U0001ee80-\U0001ee89\U0001ee8b-\U0001ee9b\U0001eea1-\U0001eea3\U0001eea5-\U0001eea9\U0001eeab-\U0001eebb\U0002a700-\U0002b734\U0002b740-\U0002b81d];

		NON_CJK = NON_ASCII | [a-zA-Z];

		non_ascii_word = (NON_ASCII+ [a-zA-Z] NON_CJK*) | ([a-zA-Z]+ NON_ASCII NON_CJK*);

		// http://www.regular-expressions.info/posixbrackets.html
		punct = [!\"\#$%&\'()*+,\-\./:;<=>\?@\[\\\]^_`{|}~];

		mixedcase = ([a-z]+ [A-Z] [a-zA-Z]* ) | ( [A-Z]+ [a-z] [a-zA-Z]*);

		uppercase = [A-Z]+;

		word = [a-zA-Z] [a-z'’()\[\]{}]*;

		nonword = [^a-zA-Z\x00:]+;

		subsentence = [:.;\?!];

		opening_quote 		= ["'] |
							"\u20bb" | ([\xc2][\xbb]) |
							"\u2018" | ([\xe2][\x80][\x98]) |
							"\u201c" | ([\xe2][\x80][\x9c]) |
							"\u201e" | ([\xe2][\x80][\x9e]) |
							"\u201a" | ([\xe2][\x80][\x9a]) |
							"\u203a" | ([\xe2][\x80][\xba]) ;

		closing_quote		= ["'] |
							"\u201d" | ([\xe2][\x80][\x9d]) |
							"\u2033" | ([\xe2][\x80][\xb3]) |
							"\u00ab" | ([\xc2][\xab]) |
							"\u00bc" | ([\xca][\xbc]) |
							"\u02ee" | ([\xcb][\xae]) |
							"\u2019" | ([\xe2][\x80][\x99]) |
							"\u2039" | ([\xe2][\x80][\xb9]) ;


		start_phrase = [ ] ([“‘(\[] | opening_quote ) [ \t]*;

		end_phrase = ([’”)\]] | closing_quote) [ \t]+;

		space = [ \t]+;

		apostrophe = "'"|([\xE2][\x80][\x99])|"\u2019";

		apos = apostrophe [a-z]*;

		other_word = [a-zA-Z] [a-zA-Z'’()\[\]{}]*;

		file_path = [ ] [/\\] [a-zA-Z]+ [a-zA-Z\-_/\\]+;

		url = [a-zA-Z\-_]+ [@.:] [a-zA-Z\-_@.:/]+ apos?;

		last_hyphen = '-' word space? '\x00';

		non_ascii_word			{ return WORD_UTF8; }

		[0-9] [0-9a-zA-Z]+		{ return WORD_MIXED; }

		file_path | url			{ return WORD_URL; }

		'AT&T' apos?			{ return WORD_UPPER; }

		'Q&A' apos?				{ return WORD_UPPER; }

		shortwords / (punct* '\x00')		{ return WORD_LAST; }

		shortwords / (punct* end_phrase) { return WORD_LAST; }

		shortwords / ':' 	{ return WORD_LAST; }

		shortwords / subsentence 	{ return WORD_LAST; }

		shortwords / last_hyphen 	{ return WORD_LAST; }

		shortwords / nonword	{ return WORD_SHORT; }

		uppercase apos?			{ return WORD_UPPER; }

		mixedcase apos?			{ return WORD_MIXED; }

		word apos?				{ return WORD_PLAIN; }

		other_word apos?		{ return WORD_MIXED; }

		start_phrase			{ return START_SUBPHRASE; }

		subsentence	/ space		{ return START_SUBSENTENCE; }

		end_phrase				{ return END_SUBPHRASE; }

		punct					{ return PUNCT; }

		// Skip over anything else
		. 						{ goto scan; }
	*/
}


int utf8_word_type(const char * str, size_t len) {
	utf8proc_uint8_t * lower = NULL;
	utf8proc_int32_t utf8_char = 0;
	utf8proc_ssize_t utf8_pre_len = 0;
	int result = WORD_MIXED;

	utf8proc_map((const utf8proc_uint8_t *) str, len, &lower, UTF8PROC_CASEFOLD);

	// Extract first character for testing
	utf8_pre_len = utf8proc_iterate((const utf8proc_uint8_t *) str, len, &utf8_char);

	if (strncmp((const char *)lower, str, len) == 0) {
		// Word is already lower case
		result = WORD_PLAIN;
	} else if (strncmp((const char *)lower + utf8_pre_len, str + utf8_pre_len, len - utf8_pre_len) == 0) {
		// Word is already title case
		result = WORD_PLAIN;
	}

	free(lower);

	return result;
}


char * title_case_string(const char * str) {
	DString * result = d_string_new("");

	// Stop at first line ending, if present
	char * stop = (char *) str;

	while (!char_is_line_ending(*stop)) {
		stop++;
	}

	char * title = title_case_string_len(str, stop - str);

	d_string_append(result, title);

	// Add anything we did not parse back to end
	while (*stop != '\0') {
		d_string_append_c(result, *stop);
		stop++;
	}

	// Clean up
	free(title);
	title = result->str;
	d_string_free(result, false);

	return title;
}


char * title_case_string_len(const char * str, size_t len) {
	DString * out = d_string_new("");
	utf8proc_int32_t utf8_char = 0;
	utf8proc_ssize_t utf8_pre_len = 0;
	utf8proc_ssize_t utf8_post_len = 0;
	unsigned char * encoded[4];

	DString * in = d_string_new(str);
	bool lc = true;
	char * test = in->str;

	// Where do we stop parsing?
	const char * stop = in->str + len;

	while (lc && test < stop) {
		if (*test >= 'a' && *test <= 'z') {
			lc = false;
		}

		test++;
	}

	if (lc) {
		test = in->str;

		while (test < stop) {
			*test = tolower(*test);

			test++;
		}
	}

	// Create a scanner (for re2c)
	Scanner s;
	s.start = in->str;
	s.cur = s.start;

	int type;								// TOKEN type

	bool first = true;
	bool upper;

	const char * last_stop = in->str;
	stop = in->str + len;

	do {
		// Scan for next word
		type = scan(&s, stop);

		if (type &&
			(s.start != last_stop) &&
			(stop > last_stop)) {
			d_string_append_c_array(out, last_stop, (int)(s.start - last_stop));
		}

		switch (type) {
			case 0:
				// Finished
				d_string_append_c_array(out, last_stop, (int)(stop - last_stop));
				break;

			case WORD_SHORT:
				if (first) {
					// Capitalize short words at beginning of title
					print_cap;
				} else {
					// Otherwise lower case them
					print_lower;
				}

				break;

			case WORD_UPPER:
				// Already upper case
				print_upper;
				break;

			case WORD_LAST:
			case WORD_PLAIN:
				// Capitalize any word at end of title
				print_cap;
				break;

			case START_SUBSENTENCE:
			case START_SUBPHRASE:
				first = true;

			case END_SUBPHRASE:
			case PUNCT:
			case WORD_MIXED:
			case WORD_URL:
				print_as_is;
				break;

			case WORD_UTF8:
				type = utf8_word_type(s.start, s.cur - s.start);

				switch (type) {
					case WORD_PLAIN:
						// Capitalize this
						utf8_pre_len = utf8proc_iterate((const utf8proc_uint8_t *) s.start, s.cur - s.start, &utf8_char);
						utf8_char = utf8proc_toupper(utf8_char);

						utf8_post_len = utf8proc_encode_char(utf8_char, (unsigned char*) encoded);

						d_string_append_c_array(out, (const char *) encoded, utf8_post_len);

						d_string_append_c_array(out, s.start + utf8_pre_len, s.cur - s.start - utf8_pre_len );
						break;
					case WORD_MIXED:
						// Print as is
						print_as_is;
						break;
				}

				break;
		}

		if (first && type == PUNCT) {

		} else if (type != START_SUBSENTENCE && type != START_SUBPHRASE) {
			first = false;
		}

		// Remember where token ends to detect skipped characters
		last_stop = s.cur;
	} while (type != 0);

	// Trim leading/lagging whitespace
	while (char_is_whitespace(*(out->str))) {
		d_string_erase(out, 0, 1);
	}

	while (out->currentStringLength && char_is_whitespace(out->str[out->currentStringLength - 1])) {
		d_string_erase(out, out->currentStringLength - 1, 1);
	}

	char * result = out->str;
	d_string_free(out, false);
	d_string_free(in, true);

	return result;
}



#ifdef TEST
void Test_title_case(CuTest * tc) {
	char * result;

	result = title_case_string("this v that");
	CuAssertStrEquals(tc, "This v That", result);
	free(result);

	result = title_case_string("this vs that");
	CuAssertStrEquals(tc, "This vs That", result);
	free(result);

	result = title_case_string("this v. that");
	CuAssertStrEquals(tc, "This v. That", result);
	free(result);

	result = title_case_string("this vs. that");
	CuAssertStrEquals(tc, "This vs. That", result);
	free(result);

	result = title_case_string("BlackBerry vs. iPhone");
	CuAssertStrEquals(tc, "BlackBerry vs. iPhone", result);
	free(result);

	result = title_case_string("Small word at end is nothing to be afraid of");
	CuAssertStrEquals(tc, "Small Word at End Is Nothing to Be Afraid Of", result);
	free(result);

	result = title_case_string("a thing");
	CuAssertStrEquals(tc, "A Thing", result);
	free(result);

	result = title_case_string("  this is trimming");
	CuAssertStrEquals(tc, "This Is Trimming", result);
	free(result);

	result = title_case_string("this is trimming  ");
	CuAssertStrEquals(tc, "This Is Trimming", result);
	free(result);

	result = title_case_string("  this is trimming  ");
	CuAssertStrEquals(tc, "This Is Trimming", result);
	free(result);

	result = title_case_string("Starting sub-phrase with a small word: a trick, perhaps?");
	CuAssertStrEquals(tc, "Starting Sub-Phrase With a Small Word: A Trick, Perhaps?", result);
	free(result);

	result = title_case_string("Sub-phrase with a small word in quotes: 'a trick, perhaps?'");
	CuAssertStrEquals(tc, "Sub-Phrase With a Small Word in Quotes: 'A Trick, Perhaps?'", result);
	free(result);

	result = title_case_string("Sub-phrase with a small word in quotes: 'a trick, perhaps?'");
	CuAssertStrEquals(tc, "Sub-Phrase With a Small Word in Quotes: 'A Trick, Perhaps?'", result);
	free(result);

	result = title_case_string("Sub-phrase with a small word in quotes: \"a trick, perhaps?\"");
	CuAssertStrEquals(tc, "Sub-Phrase With a Small Word in Quotes: \"A Trick, Perhaps?\"", result);
	free(result);

	result = title_case_string("\"Nothing to Be Afraid of?\"");
	CuAssertStrEquals(tc, "\"Nothing to Be Afraid Of?\"", result);
	free(result);

	result = title_case_string("Dr. Strangelove (or: how I Learned to Stop Worrying and Love the Bomb)");
	CuAssertStrEquals(tc, "Dr. Strangelove (Or: How I Learned to Stop Worrying and Love the Bomb)", result);
	free(result);

	result = title_case_string("Apple deal with at&T falls through");
	CuAssertStrEquals(tc, "Apple Deal With AT&T Falls Through", result);
	free(result);

	result = title_case_string("What could/should be done about slashes?");
	CuAssertStrEquals(tc, "What Could/Should Be Done About Slashes?", result);
	free(result);

	result = title_case_string("'by the way, small word at the start but within quotes.'");
	CuAssertStrEquals(tc, "'By the Way, Small Word at the Start but Within Quotes.'", result);
	free(result);

	result = title_case_string("Have you read “The Lottery”?");
	CuAssertStrEquals(tc, "Have You Read “The Lottery”?", result);
	free(result);

	result = title_case_string("Q&A with Steve Jobs: 'That's what happens in technology'");
	CuAssertStrEquals(tc, "Q&A With Steve Jobs: 'That's What Happens in Technology'", result);
	free(result);

	result = title_case_string("Q&A with Steve Jobs: 'That's what happens in technology'");
	CuAssertStrEquals(tc, "Q&A With Steve Jobs: 'That's What Happens in Technology'", result);
	free(result);

	result = title_case_string("What is AT&T's problem?");
	CuAssertStrEquals(tc, "What Is AT&T's Problem?", result);
	free(result);

	result = title_case_string("The SEC's Apple probe: what you need to know");
	CuAssertStrEquals(tc, "The SEC's Apple Probe: What You Need to Know", result);
	free(result);

	result = title_case_string("Notes and observations regarding Apple’s announcements from ‘The Beat Goes On’ special event");
	CuAssertStrEquals(tc, "Notes and Observations Regarding Apple’s Announcements From ‘The Beat Goes On’ Special Event", result);
	free(result);

	result = title_case_string("IF IT’S ALL CAPS, FIX IT");
	CuAssertStrEquals(tc, "If It’s All Caps, Fix It", result);
	free(result);

	result = title_case_string("Have you read “The Lottery”?");
	CuAssertStrEquals(tc, "Have You Read “The Lottery”?", result);
	free(result);

	result = title_case_string("your hair[cut] looks (nice)");
	CuAssertStrEquals(tc, "Your Hair[cut] Looks (Nice)", result);
	free(result);

	result = title_case_string("For step-by-step directions email someone@gmail.com");
	CuAssertStrEquals(tc, "For Step-by-Step Directions Email someone@gmail.com", result);
	free(result);

	result = title_case_string("People probably won't put http://foo.com/bar/ in titles");
	CuAssertStrEquals(tc, "People Probably Won't Put http://foo.com/bar/ in Titles", result);
	free(result);

	result = title_case_string("Scott Moritz and TheStreet.com’s million iPhone la‑la land");
	CuAssertStrEquals(tc, "Scott Moritz and TheStreet.com’s Million iPhone La‑La Land", result);
	free(result);

	result = title_case_string("Read markdown_rules.txt to find out how _underscores around words_ will be interpretted");
	CuAssertStrEquals(tc, "Read markdown_rules.txt to Find Out How _Underscores Around Words_ Will Be Interpretted", result);
	free(result);

	result = title_case_string("2lmc Spool: 'Gruber on OmniFocus and Vapo(u)rware'");
	CuAssertStrEquals(tc, "2lmc Spool: 'Gruber on OmniFocus and Vapo(u)rware'", result);
	free(result);

	result = title_case_string("Never touch paths like /var/run before/after /boot");
	CuAssertStrEquals(tc, "Never Touch Paths Like /var/run Before/After /boot", result);
	free(result);

	// Additional test cases from https://github.com/MatthewMi11er/js-title-case/blob/master/test/test-cases.js

	result = title_case_string("follow step-by-step instructions");
	CuAssertStrEquals(tc, "Follow Step-by-Step Instructions", result);
	free(result);

	result = title_case_string("this sub-phrase is nice");
	CuAssertStrEquals(tc, "This Sub-Phrase Is Nice", result);
	free(result);

	result = title_case_string("catchy title: a subtitle");
	CuAssertStrEquals(tc, "Catchy Title: A Subtitle", result);
	free(result);

	result = title_case_string("catchy title: \"a quoted subtitle\"");
	CuAssertStrEquals(tc, "Catchy Title: \"A Quoted Subtitle\"", result);
	free(result);

	result = title_case_string("catchy title: “‘a twice quoted subtitle’”");
	CuAssertStrEquals(tc, "Catchy Title: “‘A Twice Quoted Subtitle’”", result);
	free(result);

	result = title_case_string("\"a title inside double quotes\"");
	CuAssertStrEquals(tc, "\"A Title Inside Double Quotes\"", result);
	free(result);

	result = title_case_string("all words capitalized");
	CuAssertStrEquals(tc, "All Words Capitalized", result);
	free(result);

	result = title_case_string("small words are for by and of lowercase");
	CuAssertStrEquals(tc, "Small Words Are for by and of Lowercase", result);
	free(result);

	result = title_case_string("a small word starts");
	CuAssertStrEquals(tc, "A Small Word Starts", result);
	free(result);

	result = title_case_string("a small word it ends on");
	CuAssertStrEquals(tc, "A Small Word It Ends On", result);
	free(result);

	result = title_case_string("do questions work?");
	CuAssertStrEquals(tc, "Do Questions Work?", result);
	free(result);

	result = title_case_string("multiple sentences. more than one.");
	CuAssertStrEquals(tc, "Multiple Sentences. More Than One.", result);
	free(result);

	result = title_case_string("Ends with small word of");
	CuAssertStrEquals(tc, "Ends With Small Word Of", result);
	free(result);

	result = title_case_string("double quoted \"inner\" word");
	CuAssertStrEquals(tc, "Double Quoted \"Inner\" Word", result);
	free(result);

	result = title_case_string("single quoted 'inner' word");
	CuAssertStrEquals(tc, "Single Quoted 'Inner' Word", result);
	free(result);

	result = title_case_string("fancy double quoted “inner” word");
	CuAssertStrEquals(tc, "Fancy Double Quoted “Inner” Word", result);
	free(result);

	result = title_case_string("fancy single quoted ‘inner’ word");
	CuAssertStrEquals(tc, "Fancy Single Quoted ‘Inner’ Word", result);
	free(result);

	result = title_case_string("this vs. that");
	CuAssertStrEquals(tc, "This vs. That", result);
	free(result);

	result = title_case_string("this vs that");
	CuAssertStrEquals(tc, "This vs That", result);
	free(result);

	result = title_case_string("this v. that");
	CuAssertStrEquals(tc, "This v. That", result);
	free(result);

	result = title_case_string("this v that");
	CuAssertStrEquals(tc, "This v That", result);
	free(result);

	result = title_case_string("address email@example.com titles");
	CuAssertStrEquals(tc, "Address email@example.com Titles", result);
	free(result);

	result = title_case_string("pass camelCase through");
	CuAssertStrEquals(tc, "Pass camelCase Through", result);
	free(result);

	result = title_case_string("don't break");
	CuAssertStrEquals(tc, "Don't Break", result);
	free(result);

	result = title_case_string("catchy title: substance subtitle");
	CuAssertStrEquals(tc, "Catchy Title: Substance Subtitle", result);
	free(result);

	result = title_case_string("we keep NASA capitalized");
	CuAssertStrEquals(tc, "We Keep NASA Capitalized", result);
	free(result);

	result = title_case_string("leave Q&A unscathed");
	CuAssertStrEquals(tc, "Leave Q&A Unscathed", result);
	free(result);

	result = title_case_string("Scott Moritz and TheStreet.com’s million iPhone la-la land");
	CuAssertStrEquals(tc, "Scott Moritz and TheStreet.com’s Million iPhone La-La Land", result);
	free(result);

	result = title_case_string("you have a http://example.com/foo/ title");
	CuAssertStrEquals(tc, "You Have a http://example.com/foo/ Title", result);
	free(result);

	result = title_case_string("your hair[cut] looks (nice)");
	CuAssertStrEquals(tc, "Your Hair[cut] Looks (Nice)", result);
	free(result);

	result = title_case_string("keep that colo(u)r");
	CuAssertStrEquals(tc, "Keep That Colo(u)r", result);
	free(result);

	result = title_case_string("have you read “The Lottery”?");
	CuAssertStrEquals(tc, "Have You Read “The Lottery”?", result);
	free(result);

	result = title_case_string("Read markdown_rules.txt to find out how _underscores around words_ will be interpreted");
	CuAssertStrEquals(tc, "Read markdown_rules.txt to Find Out How _Underscores Around Words_ Will Be Interpreted", result);
	free(result);

	result = title_case_string("Read markdown_rules.txt to find out how *asterisks around words* will be interpreted");
	CuAssertStrEquals(tc, "Read markdown_rules.txt to Find Out How *Asterisks Around Words* Will Be Interpreted", result);
	free(result);

	result = title_case_string("Notes and observations regarding Apple’s announcements from ‘The Beat Goes On’ special event");
	CuAssertStrEquals(tc, "Notes and Observations Regarding Apple’s Announcements From ‘The Beat Goes On’ Special Event", result);
	free(result);

	result = title_case_string("take them on: special lower cases");
	CuAssertStrEquals(tc, "Take Them On: Special Lower Cases", result);
	free(result);

	result = title_case_string("capitalize hyphenated words on-demand");
	CuAssertStrEquals(tc, "Capitalize Hyphenated Words On-Demand", result);
	free(result);

	result = title_case_string("## 3.9 Foo - Bar 4");
	CuAssertStrEquals(tc, "## 3.9 Foo - Bar 4", result);
	free(result);

	result = title_case_string("### 4. foo");
	CuAssertStrEquals(tc, "### 4. Foo", result);
	free(result);

	result = title_case_string("# tests this is # ");
	CuAssertStrEquals(tc, "# Tests This Is #", result);
	free(result);

	result = title_case_string("# tests this is #  \n\n\n\n");
	CuAssertStrEquals(tc, "# Tests This Is #\n\n\n\n", result);
	free(result);

	// Non-ASCII characters
	result = title_case_string("Drink this piña colada while you listen to ænima");
	CuAssertStrEquals(tc, "Drink This Piña Colada While You Listen to Ænima", result);
	free(result);

	result = title_case_string("bücher BÜCHER BÜCher");
	CuAssertStrEquals(tc, "Bücher BÜCHER BÜCher", result);
	free(result);

	// I don't support changing case of raw HTML

//	result = title_case_string("<h1>some <b>HTML</b> &amp; entities</h1>");
//	CuAssertStrEquals(tc, "<h1>Some <b>HTML</b> &amp; Entities</h1>", result);
//	free(result);

}

#endif

