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

// Basic scanner struct

#define YYCTYPE			unsigned char
#define YYCURSOR		s->cur
#define YYMARKER		s->ptr
#define YYCTXMARKER		s->ctx


#define print_cap		d_string_append_c(out, toupper(*(s.start))); for (size_t i = (size_t)(s.start + 1 - str); i < (s.cur - str); i++) { d_string_append_c(out, tolower(str[i])); }
#define print_lower		d_string_append_c(out, tolower(*(s.start))); d_string_append_c_array(out, s.start + 1, (int)(s.cur - s.start - 1));
#define print_as_is		d_string_append_c_array(out, s.start, (int)(s.cur - s.start));
#define print_upper		upper = true; for (size_t i = (size_t)(s.start - str); i < (s.cur - str); i++) { if (str[i] == '\'') {upper = false; } if (upper) {d_string_append_c(out, toupper(str[i]));} else {d_string_append_c(out, str[i]); } }


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


char * title_case_string(const char * str) {
	size_t len = strlen(str);

	return title_case_string_len(str, len);
}


char * title_case_string_len(const char * str, size_t len) {
	DString * out = d_string_new("");

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
					print_cap;
				} else {
					print_lower;
				}

				break;

			case WORD_UPPER:
				print_upper;
				break;

			case WORD_LAST:
			case WORD_PLAIN:
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
		}

//		if (type) {
//			fprintf(stderr, "%d: '%.*s'\n", type, (int)(s.cur - s.start), s.start);
//		}

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


	// I don't support titlecase raw HTML or complex unicode characters

//	result = title_case_string("Drink this piña colada while you listen to ænima");
//	CuAssertStrEquals(tc, "Drink This Piña Colada While You Listen to Ænima", result);
//	free(result);

//	result = title_case_string("<h1>some <b>HTML</b> &amp; entities</h1>");
//	CuAssertStrEquals(tc, "<h1>Some <b>HTML</b> &amp; Entities</h1>", result);
//	free(result);

}

#endif

