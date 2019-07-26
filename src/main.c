/*

	main.c -- Template main()

	Copyright © 2015-2016 Fletcher T. Penney.


	This program is free software you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "d_string.h"
#include "libTitleCase.h"

#define kBUFFERSIZE 4096	// How many bytes to read at a time

DString * stdin_buffer() {
	/* Read from stdin and return a GString *
		`buffer` will need to be freed elsewhere */

	char chunk[kBUFFERSIZE];
	size_t bytes;

	DString * buffer = d_string_new("");

	while ((bytes = fread(chunk, 1, kBUFFERSIZE, stdin)) > 0) {
		d_string_append_c_array(buffer, chunk, bytes);
	}

	fclose(stdin);

	return buffer;
}

/// Read file into memory
DString * scan_file(const char * fname) {
	/* Read from stdin and return a DString *
	 `buffer` will need to be freed elsewhere */

	char chunk[kBUFFERSIZE];
	size_t bytes;

	FILE * file;

#if defined(__WIN32)
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, fname, -1, NULL, 0);
	wchar_t wstr[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, fname, -1, wstr, wchars_num);

	if ((file = _wfopen(wstr, L"rb")) == NULL) {
		return NULL;
	}

#else

	if ((file = fopen(fname, "r")) == NULL ) {
		return NULL;
	}

#endif

	DString * buffer = d_string_new("");

	while ((bytes = fread(chunk, 1, kBUFFERSIZE, file)) > 0) {
		d_string_append_c_array(buffer, chunk, bytes);
	}

	// Strip UTF-8 BOM
	if (strncmp(buffer->str, "\xef\xbb\xbf", 3) == 0) {
		d_string_erase(buffer, 0, 3);
	}

	// Strip UTF-16 BOMs
	if (strncmp(buffer->str, "\xef\xff", 2) == 0) {
		d_string_erase(buffer, 0, 2);
	}

	if (strncmp(buffer->str, "\xff\xfe", 2) == 0) {
		d_string_erase(buffer, 0, 2);
	}

	fclose(file);

	return buffer;
}

<<< <<< < HEAD
int main( int argc, char ** argv ) {
	== == == =

	int main( int argc, char ** argv ) {
		>>> >>> > 3890a4de95421896ac7874f043728511443d221f

		if (argc == 1) {
			DString * text = stdin_buffer();

			char * result = title_case_string_len(text->str, text->currentStringLength);

			fprintf(stdout, "%s\n", result);

			d_string_free(text, true);
			free(result);
		} else  if (argc == 2) {
			char * text = argv[1];

			char * result = title_case_string(text);

			fprintf(stdout, "%s\n", result);

			free(result);
		}
	}
