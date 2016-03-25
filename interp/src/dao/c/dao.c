#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_BIT_SIZE (unsigned long)(1 << 17)

#define FILE_SYMBOLIC ".dao"
#define FILE_COMPILED ".wuwei"

#define IDLES 0x0
#define SWAPS 0x1
#define LATER 0x2
#define MERGE 0x3
#define SIFTS 0x4
#define EXECS 0x5
#define DELEV 0x6
#define EQUAL 0x7
#define HALVE 0x8
#define UPLEV 0x9
#define READS 0xA
#define DEALC 0xB
#define SPLIT 0xC
#define POLAR 0xD
#define DOALC 0xE
#define INPUT 0xF

typedef struct PATH* Path;
typedef char* String;

void compile(FILE*, FILE*);
void idles(Path); void swaps(Path); void later(Path); void merge(Path);
void sifts(Path); void execs(Path, Path); void delev(Path); void equal(Path);
void halve(Path); void uplev(Path); void reads(Path); void dealc(Path);
void split(Path); void polar(Path); void doalc(Path); void input(Path);

char getChar(unsigned char);
unsigned char getNybble(char);
unsigned long mask(int);
unsigned long report_by_bit_index(Path, unsigned int, unsigned int);

void write_by_bit_index(Path, unsigned int, unsigned int, unsigned long);

char*			bin(unsigned long);
char*			itoa(unsigned long, unsigned char, unsigned char);
void			bin_print(Path);
void			skip();

int aligned(Path);

struct PATH
{
	Path 			owner;						/* OWNER   PROGRAM     */
	Path 			child;						/* CHILD   PROGRAM     */

	unsigned long	prg_data[ MAX_BIT_SIZE ];	/*         DATA        */
	unsigned long	prg_allocbits;				/* OPEN    DATA   BITS */
	unsigned long	prg_index;					/* INSTRUCTION  POINTER*/
	unsigned char	prg_level;					/* OPERATING LEVEL     */
	
	unsigned long	sel_length;					/* LENGTH OF SELECTION */
	unsigned long	sel_index;					/* INDEX  OF SELECTION */
};

const struct PATH NEW_PATH =
{
	NULL, NULL,
	{0}, 1, 0, 0, 1, 0
};

#define scan_by_char(c,f,x) while((c = fgetc(f)) != EOF) {x}
#define wheel(k, n, x) if (++k == n) {k = 0; x}
#define verp(x) verx{printf(x);}
#define verx if (VERBOSE)

static char VERBOSE = 0;
static char HURRY = 0;
static char FORCE = 0;

static Path P_RUNNING;

int main(int argc, char * const argv[])
{
	if (argc < 2)
	{
		printf("\t\t#################################\n");
		printf("\t\t##############     ##############\n");
		printf("\t\t#########     #####     #########\n");
		printf("\t\t######   ###############   ######\n");
		printf("\t\t####   ###################   ####\n");
		printf("\t\t###  #######################  ###\n");
		printf("\t\t##  #####   ########====#####  ##\n");
		printf("\t\t#  #####     #####==    ===###  #\n");
		printf("\t\t# =#####     ####=         ==## #\n");
		printf("\t\t#+ =#####   ####=    +++     =# #\n");
		printf("\t\t#+  ==#########=    +++++     =+#\n");
		printf("\t\t#++   ===####==     +++++     ++#\n");
		printf("\t\t##++     ====        +++     ++##\n");
		printf("\t\t###++                       ++###\n");
		printf("\t\t####+++                   +++####\n");
		printf("\t\t######+++               +++######\n");
		printf("\t\t#########+++++     +++++#########\n");
		printf("\t\t##############+++++##############\n");
		printf("\t\t#################################\n");
		printf("\t\t###===========================###\n");
		printf("\t\t##|      D      A      O      |##\n");
		printf("\t\t###===========================###\n");
		printf("\t\t#################################\n");
		printf("\r\n[Welcome to DAOLANGUAGE UTILITY ver 0.9.9.9]\n");
		printf("\tPlease remember to enter a filename as a parameter, for example:\n\n");
		printf("\t\"> dao hello_world.dao\"\n");
		printf("\t\tto compile, or...\n\n");
		printf("\t\"> dao hello_world.wuwei\"\n");
		printf("\t\tto execute.\n\n");
		printf("Options:\n");
		printf("\t-v   :   Enable Verbose Execution (For Debugging)\n");
		printf("\t-h   :   Run Immediately After Compiling\n");
		printf("\t-f   :   Force Execution of Any File as DAOLANGUAGE (DANGEROUS)\n");
		printf("\n");
		return(0);
	}
	else
	{
		String inputFileName = argv[1];
		FILE* inputFile = fopen(inputFileName,"rb");
		int c;
		opterr = 0;
  		while ((c = getopt(argc, argv, "vhf")) != -1)
    		switch (c)
      		{
      		case 'v':	VERBOSE = 1;	break;
        	case 'h':	HURRY = 1;		break;
        	case 'f':	FORCE = 1;		break;
        	case '?':
        		printf("Unknown option -%c.\n\n", optopt);
        		break;
      		}

		if (inputFile == NULL)
		{
			printf("Could not find \"%s\" - is it in this directory?\n", inputFileName);
			fclose(inputFile);
			return 1;
		}
		if (~strcmp(FILE_SYMBOLIC, &inputFileName[strlen(inputFileName)-4]))
		{
			FILE* outputFile;
			inputFileName[strlen(inputFileName)-4] = 0;
			inputFileName = strncat(inputFileName, FILE_COMPILED, sizeof(FILE_COMPILED));
			outputFile = fopen(inputFileName,"w+");
			verx printf("\n%s%s\n", "Compiling symbolic dao to ", inputFileName);
			compile(inputFile, outputFile);
			verp("Finished compiling.");
			fclose(inputFile);
			fclose(outputFile);
			if (HURRY)
			{
				inputFile = fopen(inputFileName,"rb");
				if (VERBOSE)
				{
					printf("\n\n\t=====================\n");
					printf(    "\t|Beginning Execution|\n");
					printf(    "\t=====================\n\n");
				}
			}
			else
				return 0;
		}
		if (FORCE || ~strcmp(FILE_COMPILED, &inputFileName[strlen(inputFileName)-6]))
		{
			int c;
			int i = 0;
			int shift = 0;
			int j = 0;
			int k = 0;				
			struct PATH newPath = NEW_PATH;
			Path dao = &newPath;	
			if (VERBOSE)
				printf("%s%s.\nLoading data:\n", "Running ", inputFileName);	
			scan_by_char(c, inputFile,
				(dao -> prg_data)[i] |= ((unsigned long)c << ((3 - shift) * 8));
				wheel(shift, 4, i++;)
			)
			if (feof(inputFile))
			{
  				if (VERBOSE) printf("Hit end of file at position %x.\n\n", i*4 + shift);
			}
			else
			{
  				printf("Encountered an error during file read.\n");
  				return(22);
			}	
			while (j < (i + ((shift + 3) / 4)))
			{
				if (VERBOSE)
				{
					printf("%x   ", (dao -> prg_data)[j]);
					wheel(k, 7, printf("\n");)
				}
				j++;
			}
			if (VERBOSE)
				printf("(%d bytes)\n\n", 4 * j);	
			while ((dao -> prg_allocbits) / 32 < j)
				(dao -> prg_allocbits) *= 2;
			P_RUNNING = dao;				
			execs(dao, NULL);
		}
	}
	return 0;
}

void compile(FILE* input, FILE* output)
{
	unsigned char emptyBuffer = 1;
	unsigned char toWrite = 0;
	unsigned char isComment = 0;
	int ch;
	scan_by_char(ch, input,
		switch((char)ch)
		{
		case '\t':			case ' ':			break;
   		case '@': 			isComment = 1; 		break; 
   		case (char)0x0D:
   		case (char)0x0A: 	isComment = 0; 		break;
		default:
			if (!isComment)
			{
				if (VERBOSE)
					putchar((char)ch);
				if (!emptyBuffer)
				{
					toWrite |= getNybble((char)ch);
					fputc(toWrite, output);
					emptyBuffer = 1;
					if (VERBOSE)
						printf(" %x\n", toWrite);
				}
				else
				{
					toWrite = (char)(getNybble((char)ch) << 4);
					emptyBuffer = 0;
				}
			}
			break;
		}
	)
	if (!emptyBuffer) {
		fputc(toWrite, output);
		if (VERBOSE)
			printf(". %x\n", toWrite);
	}
}

unsigned char getNybble(char ch)
{
   switch (ch) 
   {
		case '.': return IDLES;	case '!': return SWAPS;	case '/': return LATER; case ']': case ')': return MERGE;	
		case '%': return SIFTS;	case '#': return EXECS;	case '>': return DELEV;	case '=': return EQUAL;	
		case '(': return HALVE;	case '<': return UPLEV;	case ':': return READS; case 'S': return DEALC;	
		case '[': return SPLIT;	case '*': return POLAR;	case '$': return DOALC;	case ';': return INPUT;
		default: return IDLES;
	}
}

char getChar(unsigned char ch)
{
   switch (ch) 
   {
		case IDLES: return '.';	case SWAPS: return '!';	case LATER: return '/'; case MERGE: return ')';
		case SIFTS: return '%';	case EXECS: return '#';	case DELEV: return '>';	case EQUAL: return '=';
		case HALVE: return '(';	case UPLEV: return '<';	case READS: return ':'; case DEALC: return 'S';
		case SPLIT: return '[';	case POLAR: return '*';	case DOALC: return '$';	case INPUT: return ';';
		default: return '?';
	}
}

char* bin(unsigned long val)
{
	return itoa(val, 32, 2);
}

char* itoa(unsigned long val, unsigned char len, unsigned char radix)
{
	static char buf[32] = {'0'};
	int i = 33;
	for(; val && i ; --i, val /= radix)
		buf[i] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[val % radix];
	for(; i ; i--)
		buf[i] = '0';
	return &buf[2+(32-len)];
}
/*█████████████████████████████████████████████*/
/*████████████████████     ████████████████████*/
/*███████████████     █████     ███████████████*/
/*████████████   ███████████████   ████████████*/
/*██████████   ███████████████████   ██████████*/
/*█████████  ███████████████████████  █████████*/
/*████████  █████   ████████▒▒▒▒█████  ████████*/
/*███████  █████     █████▒▒░░░░▒▒▒███  ███████*/
/*███████ ▒█████     ████▒░░░░░░░░░▒▒██ ███████*/
/*███████▓░▒█████   ████▒░░░░▓▓▓░░░░░▒█ ███████*/
/*███████▓░░▒▒█████████▒░░░░▓▓▓▓▓░░░░░▒▓███████*/
/*███████▓▓░░░▒▒▒████▒▒░░░░░▓▓▓▓▓░░░░░▓▓███████*/
/*████████▓▓░░░░░▒▒▒▒░░░░░░░░▓▓▓░░░░░▓▓████████*/
/*█████████▓▓░░░░░░░░░░░░░░░░░░░░░░░▓▓█████████*/
/*██████████▓▓▓░░░░░░░░░░░░░░░░░░░▓▓▓██████████*/
/*████████████▓▓▓░░░░░░░░░░░░░░░▓▓▓████████████*/
/*███████████████▓▓▓▓▓░░░░░▓▓▓▓▓███████████████*/
/*████████████████████▓▓▓▓▓████████████████████*/
/*█████████████████████████████████████████████*/
/*████████╬═══════════════════════════╬████████*/
/*████████║      D      A      O      ║████████*/
/*████████╬═══════════════════════════╬████████*/
/*█████████████████████████████████████████████*/

#define CELL 32

#define levlim(l)		if ((PR_LEV) >= l) {if(VERBOSE)printf("LEV_SKIP");return;}
#define levconsk(l,x)	if ((PR_LEV) < l) {x}
#define levcons(l,x,y)	if ((PR_LEV) < l) {x} else {y}

#define intIndex 		( P_IND % CELL )
#define arrIndex 		( P_IND / CELL )

#define P_LEN 			(path -> sel_length)
#define P_IND 			(path -> sel_index)

#define P_ALC 			(path -> prg_allocbits)
#define P_LEV 			(path -> prg_level)
#define P_PIND			(path -> prg_index)
#define P_DATA 			(path -> prg_data)

#define P_OWNER			(path -> owner)
#define P_CHILD			(path -> child)

#define PR_LEV 			(P_RUNNING -> prg_level)

static Path P_WRITTEN;
static unsigned int floor = 0;

void idles(Path path) {}

void swaps(Path path)
{
	levlim(2)
	if (VERBOSE)
		printf("Swapped length %d.", P_LEN);
	if (P_LEN == 1)	return;
	if (P_LEN <= CELL)
	{
		int len = P_LEN;
		int shift = CELL - intIndex - len;
		long report = (P_DATA[arrIndex] >> shift) & mask(len);
		long left = report >> (len >> 1);
		long right = report & mask(len >> 1);
		long recombine = (right << (len >> 1)) | left;
		P_DATA[arrIndex] &= ~(mask(len) << shift);
		P_DATA[arrIndex] |= recombine << shift;
	}
	else
	{
		int i = 0;
		int leftIndex = arrIndex;
		int half = (P_LEN / CELL) / 2;
		long holder;
		while (i < half)
		{
			holder = P_DATA[leftIndex + i];
			P_DATA[leftIndex + i] = P_DATA[leftIndex + half + i];
			P_DATA[leftIndex + half + i] = holder;
			i++;
		}
	}
}

void later(Path path)
{
	if (aligned(path) || (PR_LEV >= 4))
		P_IND += P_LEN;
	else
		merge(path);
}

void merge(Path path)
{
	levlim(7)
	if (P_LEN < P_ALC)
	{
		if (!aligned(path))
			P_IND -= P_LEN;
		P_LEN <<= 1;
	}
	else
	{
		if (P_OWNER != NULL)
			P_WRITTEN = P_OWNER;
	}
}

void sifts(Path path)
{
	int l = P_IND;
	levlim(5)
	while (l + 4 < P_ALC)
	{
		if (!report_by_bit_index(path, l, 4))
		{
			int r = l;
			if (VERBOSE) printf("IDLES (%s) encountered at index %d.\n", bin(report_by_bit_index(path, l, 4)), l);
			for (; report_by_bit_index(path, r, 4) && ((r + 4) < P_ALC); r++); /* Move right to first */
			for (;!report_by_bit_index(path, l, 4) && report_by_bit_index(path, r, 4) && ((r + 4) < P_ALC); l += 4, r += 4)
			{
				write_by_bit_index(path, l, 4, report_by_bit_index(path, r, 4));
				write_by_bit_index(path, r, 4, 0);
			}
		}
		l += 4;
	}
}

static unsigned char command = 0;
static int doloop = 1;

void execs(Path path, Path caller)
{
	unsigned long tempNum1 = 0;
	if (P_LEV >= 8) 
	{
		if (VERBOSE)
			printf("LEV_SKIP");
		return;
	}
	floor++;
	P_RUNNING = path;
	if (P_CHILD == NULL)
		P_CHILD = (malloc(sizeof(struct PATH)));
	if (P_CHILD == NULL)
	{
		printf("FATAL ERROR: Unable to allocate memory.");
		return;
	}

	verx
		printf("Allocated %d bytes.\n\n", sizeof(*P_CHILD));

	memcpy(P_CHILD, &NEW_PATH, sizeof(struct PATH));
	(*(*path).child).owner = path;
	P_WRITTEN = P_CHILD;
	P_PIND = (P_IND / 4);

	/* Execs Loop */
	for (; doloop && P_PIND < (P_ALC / 4); P_PIND++)
	{
		tempNum1 = (P_RUNNING -> prg_index);
		command = ((P_RUNNING -> prg_data)[(tempNum1*4) / 32] >> (32 - ((tempNum1*4) % 32) - 4)) & mask(4);

		verx
		{
			printf("%s F%x L%d %c ", itoa(P_PIND, 5, 16), floor, PR_LEV, getChar(command));
			bin_print(P_WRITTEN);
			printf(" : ");
		}

		switch(command)
		{
			case IDLES: idles(P_WRITTEN); break;
			case SWAPS: swaps(P_WRITTEN); break;
			case LATER: later(P_WRITTEN); break;
			case MERGE: merge(P_WRITTEN); break;
			case SIFTS: sifts(P_WRITTEN); break;
			case EXECS: execs(P_WRITTEN, path); break;
			case DELEV: delev(P_WRITTEN); break;
			case EQUAL: equal(P_WRITTEN); break;
			case HALVE: halve(P_WRITTEN); break;
			case UPLEV: uplev(P_WRITTEN); break;
			case READS: reads(P_WRITTEN); break;
			case DEALC: dealc(P_WRITTEN); break;
			case SPLIT: split(P_WRITTEN); break;
			case POLAR: polar(P_WRITTEN); break;
			case DOALC: doalc(P_WRITTEN); break;
			case INPUT: input(P_WRITTEN); break;
		}

		if (P_WRITTEN -> sel_index > 200)
			printf("[%d]", P_WRITTEN -> sel_index);

		verp("\n");
	}

	doloop = 1;
	if (caller != NULL)
	{
		P_RUNNING = caller;
		P_WRITTEN = caller -> child;
	}

	floor--;
	return;
}

void delev(Path path)
{
	if (PR_LEV > 0)
		PR_LEV--;
}

void equal(Path path)
{
	levlim(5)
	if (report_by_bit_index(path, P_IND, 1) ^ report_by_bit_index(path, P_IND + P_LEN - 1, 1))
			skip();
	else
		if (VERBOSE)
			printf("EQUAL");
}

void halve(Path path)
{
	levlim(7)
	if (P_LEN == 1)
	{
		if (P_CHILD != NULL)
		{
			P_WRITTEN = P_CHILD;
			(P_WRITTEN -> sel_length) = (P_WRITTEN -> prg_allocbits);
		}
		return;
	}
	P_LEN /= 2;
}

void uplev(Path path)
{
	levlim(9)
	PR_LEV++;
	(P_RUNNING -> prg_index) = 0;
}

void reads(Path path)
{
	levlim(6)
	if (P_LEN < 8)
	{
		String out = bin(report_by_bit_index(path, P_IND, P_LEN));
		printf("%s", &out[strlen(out) - P_LEN]);
		return;
	}
	else
	{
		long pos = P_IND;
		for (; pos < (P_IND + P_LEN); pos += 8)
			putchar(report_by_bit_index(path, pos, 8));
	}
} 

void dealc(Path path)
{
	levlim(2)
	if (P_ALC == 1)
	{
		int report = report_by_bit_index(path, 0, 1);
		if ((P_RUNNING -> owner) != NULL)
		{
			unsigned long ownind = ((P_RUNNING -> owner) -> prg_index);
			verx
				printf("Terminating program from position %x with value %x",\
					ownind, report);
			write_by_bit_index(P_RUNNING -> owner, (ownind) * 4, 4, report);
		}
		doloop = 0;
		return;
	}
	P_ALC >>= 1;
	halve(path);
	if ((P_IND + P_LEN) > P_ALC)
		P_IND -= P_ALC;
}

void split(Path path)
{
	if (PR_LEV < 1)
	{
		unsigned int len = P_LEN;
		if (len == 1)
		{
			if (P_CHILD != NULL)
			{
				P_WRITTEN = P_CHILD;
				(P_WRITTEN -> sel_length) = (P_WRITTEN -> prg_allocbits);
				split(P_WRITTEN);
				halve(P_WRITTEN);
			}
			return;
		}
		if (len <= CELL)
		{
			unsigned int shift = CELL - intIndex - len;
			unsigned long polarized = (mask(len >> 1) << (len >> 1));
			P_DATA[arrIndex] &= ~(mask(len) << shift);
			P_DATA[arrIndex] |= polarized << shift;
		}
		else
		{
			int leftIndex = arrIndex;
			int rightIndex = leftIndex + (len / CELL) - 1;
			while (leftIndex < rightIndex)
			{
				P_DATA[leftIndex++] = 0xFFFFFFFF;
				P_DATA[rightIndex--] = 0;
			}
		}
	}
	halve(path);
}

void polar(Path path)
{
	levlim(3)
	if (!(report_by_bit_index(path, P_IND, 1) && !report_by_bit_index(path, P_IND + P_LEN - 1, 1)))
		skip();
	else
		verp("POLAR");
}

void doalc(Path path)
{
	levlim(1)
	if (P_ALC < MAX_BIT_SIZE) {
		P_ALC <<= 1;
		merge(path);
	}
	else
	{
		printf("FATAL ERROR 1: Program exceeded maximum memory.");
		abort();
	}
}

void input(Path path)
{
	levlim(6)
	if (P_LEN < 8)
	{
		write_by_bit_index(path, P_IND, P_LEN, getchar());
		return;
	}
	else
	{
		int i = P_IND;
		for (; i < P_LEN; i += 8)
			write_by_bit_index(path, i, 8, getchar());
	}

}
/*▄▄
 ▐███▄
  ▀████▄
    ▀████▄
       ▀███▄▄
          ▀█████▄▄
              ▀▀████▄▄▄
                  ▀▀████████▄▄
                         ▀▀▀█████▄▄
                               ▀▀████▄
                                    ▀███▄▄
                                       ▀████▄
                                         ▀████▄	
                                           ▀███▌
                                             ▀▀*/
int aligned(Path path)
{
	return P_IND % (P_LEN << 1) == 0;
}

unsigned long mask(int length) 
{
	if (length < CELL)	return ((int)1 << length) - 1;
	else			 	return 0xFFFFFFFF;
} 

unsigned long report_by_bit_index(Path path, unsigned int i, unsigned int len)
{
	return (P_DATA[i / CELL] >> (CELL - (i % CELL) - len)) & mask(len);
}

void write_by_bit_index(Path path, unsigned int i, unsigned int len, unsigned long write)
{
	int shift = CELL - (i % CELL) - len;
	P_DATA[i / CELL] &= ~(mask(len) << shift);
	P_DATA[i / CELL] |= ((write & mask(len)) << shift);
}

void bin_print(Path path)
{
	long i = 0;
	if (P_ALC <= CELL)
	{
		String out = bin(report_by_bit_index(path, 0, P_ALC));
		printf("%s", &out[strlen(out) - P_ALC]);
		return;
	}
	for (; i < (P_ALC / CELL); i++)
		printf("%x", P_DATA[i]);
}

void skip()
{
	if (P_RUNNING == NULL) return;
	verp("SKIP");
	(P_RUNNING -> prg_index)++;
}