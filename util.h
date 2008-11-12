/*
    This file is part of Kismet

    Kismet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Kismet is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Kismet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __UTIL_H__
#define __UTIL_H__

#include "config.h"

#include <stdio.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pwd.h>
#include <ctype.h>
#include <math.h>

#include <string>
#include <map>
#include <vector>
#include <list>
#include <sstream>
#include <iomanip>

// ieee float struct for a 64bit float for serialization
typedef struct {
	uint64_t mantissa:52 __attribute__ ((packed));
	uint64_t exponent:11 __attribute__ ((packed));
	uint64_t sign:1 __attribute__ ((packed));
} ieee_64_float_t;

typedef struct {
	unsigned int mantissal:32;
	unsigned int mantissah:20;
	unsigned int exponent:11;
	unsigned int sign:1;
} ieee_double_t;

typedef struct {
	unsigned int mantissal:32;
	unsigned int mantissah:32;
	unsigned int exponent:15;
	unsigned int sign:1;
	unsigned int empty:16;
} ieee_long_double_t;

// Munge a string to characters safe for calling in a shell
void MungeToShell(char *in_data, int max);
string MungeToShell(string in_data);
string MungeToPrintable(const char *in_data, int max, int nullterm);
string MungeToPrintable(string in_str);

string StrLower(string in_str);
string StrUpper(string in_str);
string StrStrip(string in_str);
string StrPrintable(string in_str);
string AlignString(string in_txt, char in_spacer, int in_align, int in_width);

int HexStrToUint8(string in_str, uint8_t *in_buf, int in_buflen);
string HexStrFromUint8(uint8_t *in_buf, int in_buflen);

template<class t> class NtoString {
public:
	NtoString(t in_n, int in_precision = 0) { 
		ostringstream osstr;

		if (in_precision)
			osstr << setprecision(in_precision) << in_n;
		else
			osstr << in_n;

		s = osstr.str();
	}

	string Str() { return s; }

	string s;
};

#define IntToString(I)			NtoString<int>((I)).Str()
#define LongIntToString(L)		NtoString<long int>((L)).Str()

void SubtractTimeval(struct timeval *in_tv1, struct timeval *in_tv2,
					 struct timeval *out_tv);

// Generic options pair
struct opt_pair {
	string opt;
	string val;
};

// Generic option handlers
string FetchOpt(string in_key, vector<opt_pair> *in_vec);
vector<string> FetchOptVec(string in_key, vector<opt_pair> *in_vec);
int StringToOpts(string in_line, string in_sep, vector<opt_pair> *in_vec);
void AddOptToOpts(string opt, string val, vector<opt_pair> *in_vec);
void ReplaceAllOpts(string opt, string val, vector<opt_pair> *in_vec);

int XtoI(char x);
int Hex2UChar(unsigned char *in_hex, unsigned char *in_chr);

vector<string> StrTokenize(string in_str, string in_split, int return_partial = 1);

// 'smart' tokenizeing with start/end positions
struct smart_word_token {
    string word;
    size_t begin;
    size_t end;

    smart_word_token& operator= (const smart_word_token& op) {
        word = op.word;
        begin = op.begin;
        end = op.end;
        return *this;
    }
};
vector<smart_word_token> SmartStrTokenize(string in_str, string in_split, 
										  int return_partial = 1);
vector<smart_word_token> NetStrTokenize(string in_str, string in_split, 
										int return_partial = 1);

string InLineWrap(string in_txt, unsigned int in_hdr_len,
				  unsigned int in_max_len);
vector<string> LineWrap(string in_txt, unsigned int in_hdr_len, 
						unsigned int in_maxlen);
vector<int> Str2IntVec(string in_text);

int IsBlank(const char *s);

// Clean up XML and CSV data for output
string SanitizeXML(string);
string SanitizeCSV(string);

void Float2Pair(float in_float, int16_t *primary, int64_t *mantissa);
float Pair2Float(int16_t primary, int64_t mantissa);

// Convert a standard channel to a frequency
int ChanToFreq(int in_chan);
int FreqToChan(int in_freq);

// Convert an IEEE beacon rate to an integer # of beacons per second
unsigned int Ieee80211Interval2NSecs(int in_rate);

// Run a system command and return the error code.  Caller is responsible 
// for security.  Does not fork out
int RunSysCmd(char *in_cmd);

// Fork and exec a syscmd, return the pid of the new process
pid_t ExecSysCmd(char *in_cmd);

#ifdef SYS_LINUX
int FetchSysLoadAvg(uint8_t *in_avgmaj, uint8_t *in_avgmin);
#endif

// Adler-32 checksum, derived from rsync, adler-32
uint32_t Adler32Checksum(const char *buf1, int len);

// 802.11 checksum functions, derived from the BBN USRP 802.11 code
#define IEEE_802_3_CRC32_POLY	0xEDB88320
unsigned int update_crc32_80211(unsigned int crc, const unsigned char *data,
								int len, unsigned int poly);
void crc32_init_table_80211(unsigned int *crc32_table);
unsigned int crc32_le_80211(unsigned int *crc32_table, const unsigned char *buf, 
							int len);


// Proftpd process title manipulation functions
void init_proc_title(int argc, char *argv[], char *envp[]);
void set_proc_title(const char *fmt, ...);

// Simple lexer for "advanced" filter stuff and other tools
#define _kis_lex_none			0
#define _kis_lex_string			1
#define _kis_lex_quotestring	2
#define _kis_lex_popen			3
#define _kis_lex_pclose			4
#define _kis_lex_negate			5
#define _kis_lex_delim			6

typedef struct {
	int type;
	string data;
} _kis_lex_rec;

list<_kis_lex_rec> LexString(string in_line, string& errstr);

#endif

