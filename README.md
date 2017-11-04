## About ##

|            |                           |  
| ---------- | ------------------------- |  
| Title:     | cTitleCase        |  
| Author:    | Fletcher T. Penney       |  
| Date:      | 2017-11-04 |  
| Copyright: | Copyright © 2017 Fletcher T. Penney.    |  
| Version:   | 1.0.7      |  


## Introduction ##

Many years ago, John Gruber wrote a [Perl script] to apply title case rules to
text.  It was interesting, but I didn't have much use for it at the time.
Subsequently, it was [improved] by Aristotle Pagaltzis.  Again, I didn't need
it.

When I created MultiMarkdown Composer, however, applying title case
automatically to headers seemed like a useful feature.  Perl didn't help me,
though, and I ended up discovering Marshall Elfstrand's [Objective-C] version.
This allowed me to incorporate the functionality directly (once I fixed a
couple of bugs).

I finally decided that it was time for a proper c implementation to make the
algorithm more portable.


## Use ##

cTitleCase can be used in two ways:

1. As a library (`libTitleCase`) in your own program.  You use either
`title_case_string()` or `title_case_string_len()` depending on whether you
know the string's length or not.  Free the resulting string when you're
finished with it.

2. As a command line program (`titlecase`).  It can accept a title from the
command line or from `stdin`.  It can only accept a single line of text.

	* `titlecase "this is a title"`
	* `titlecase < title_file.txt`


I included the [test examples] that Aristotle Pagaltzis compiled (includes John
Gruber's original test examples), and it passes all of them.


## Development ##

This is a new implementation.  I use `re2c` to create a tokenizer that
recognizes words of different types, as well as punctuation that starts sub-
sentences and sub-phrases.  The program then walks through the tokens to
determine how to modify each word.

It's a relatively simple program, but still may contain bugs.  If you discover
any, please let me know.

That said, this program is not going to work perfectly for every title.  It's
limited to English.  Your mileage may vary....


## License ##
 
The `c-template` project is released under the MIT License.

d_string.c and d_string.h are from the MultiMarkdown v4 project:

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


[Perl script]:	https://daringfireball.net/2008/05/title_case
[improved]:	http://plasmasturm.org/code/titlecase/
[Objective-C]:	http://www.vengefulcow.com/titlecase/
[test examples]:	https://raw.githubusercontent.com/ap/titlecase/master/test.pl
