#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>

const unsigned int ansi[] = {
    0x00, 0x80, 0x20, 0x90,
    0x08, 0x88, 0x28, 0xa8,
    0x54, 0xd4, 0x74, 0xf4,
    0x5c, 0xdc, 0x7c, 0xfc
};

/*
const unsigned char *ansi[] = {
	"0000","2000","0200","2100",
	"0020","2020","0220","2220",
	"1110","3110","1310","3310",
	"1130","3130","1330","3330"
};
*/

/*
"\x00\x00\x00", "\xbb\x00\x00", "\x00\xbb\x00", "\xbb\xbb\x00",
"\x00\x00\xbb", "\xbb\x00\xbb", "\x00\xbb\xbb", "\xbb\xbb\xbb",
"\x55\x55\x55", "\xff\x55\x55", "\x55\xff\x55", "\xff\xff\x55",
"\x55\x55\xff", "\xff\x55\xff", "\x55\xff\xff", "\xff\xff\xff"
*/

const char *ansi_name[] = {
    "black", "red", "green", "yellow",
    "blue", "magenta", "cyan", "white",
    "hi black", "hi red", "hi green", "hi yellow",
    "hi blue", "hi magenta", "hi cyan", "hi white"
};

int bold = 0, fg = 7, bg = 0;

int xpos = 1, ypos = 1;
int xold = 1, yold = 1;

unsigned short maxypos = 32;
unsigned short maxxpos = 80;

#define MAXROWS   1000
#define MAXCOLS   80
#define MAXCOLORS 64
#define MAXFONT   128

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

unsigned short datafile[MAXROWS][MAXCOLS];

unsigned int colortable[MAXCOLORS][2];

int colortable_siz;

int fb2ct[16][8];

int byte2font[256];
int font2byte[256];

int font_siz;

char cmd[8192];

void
font_add(int ch)
{
    if (byte2font[ch] == -1) {

        assert(font_siz + 1 < MAXFONT);

        font2byte[font_siz] = ch;

        byte2font[ch] = font_siz;

        font_siz++;
    }
}

void
mapcolor(int fg0, int bg0, int bold0)
{

    /*
     *
     * when we come across a color see if we can map it
     *
     */

    if (fb2ct[fg0 + bold0 * 8][bg0] >= 0) {

        /*
         * do nothing if the current color is mapped
         */
    } else {

        /*
         * add the color to the table but if
         * we go above max colors then fail
         */

        if (colortable_siz + 1 >= MAXCOLORS) {
            fprintf(stderr, "max colors exceeded!\n");
            exit(EXIT_FAILURE);
        }

        colortable[colortable_siz][0] = fg0 + bold0 * 8;
        colortable[colortable_siz][1] = bg0;

        fb2ct[fg0 + bold0 * 8][bg0] = colortable_siz;

        colortable_siz++;
    }
}

int
main()
{

    char line[128];

    // const unsigned char extended_map[] = { 176, 177, 178, 219, 220, 221, 222, 223, 254, 250, 0 };

    int i, j, k;

    int ch;

    int onblack = -1;

    unsigned short packed;
    int color;
    int mode;

    // char *p;

    assert(sizeof(unsigned short) == 2);

    /*
     *
     * init. the fg/bg to colortable reverse lookup map
     * clear and init the colortable
     *
     */

    for (i = 0; i < 16; i++)
        for (j = 0; j < 8; j++)
            fb2ct[i][j] = -1;

    memset(colortable, sizeof(colortable), 0);

    memset(datafile, 0, sizeof(datafile));

    colortable_siz = 0;

    for (i = 0; i < 256; i++) {
        font2byte[i] = -1;
        byte2font[i] = -1;
    }

    font_siz = 0;

    for (;;) {

        (void)fgets(line, sizeof(line) - 1, stdin);

        if (feof(stdin))
            break;

        if (sscanf(line, "move %i %i", &ypos, &xpos) == 2) {

            /*
             *
             * move the cursor to the new position
             *
             */

            if (ypos > maxypos)
                maxypos = ypos;

        } else if (sscanf(line, "color bold=%i fg=%i bg=%i", &bold, &fg, &bg) == 3) {

            /*
             *
             * when we come across a color see if we can map it
             *
             */

            mapcolor(fg, bg, bold);

        } else if (sscanf(line, "print %i", &ch) == 1) {

            mapcolor(fg, bg, bold);

            color = fb2ct[fg + bold * 8][bg];

            if (color < 0 || color >= MAXCOLORS) {
                fprintf(stderr, "unmapped color!\n");
                exit(EXIT_FAILURE);
            }

            /*

               p = strchr((const char *)extended_map, ch);

               if(p) {

               mode = 1;
               ch = (unsigned long)p - (unsigned long)extended_map;

               } else {

               mode = 0;
               }

               if(ch < 128) {
               mode = 1;
               ch -= 0; // 128;
               } else {
               mode = 0;
               }

             */

            font_add(ch);

            ch = byte2font[ch];

            mode = 1;

            packed = (color << 10) | (mode << 8) | ch;

            // packed = htons(packed);

            if (ypos >= 1 && ypos <= MAXROWS && xpos >= 1 && xpos <= MAXCOLS) {
                datafile[ypos - 1][xpos - 1] = packed;
            }

        } else if (sscanf(line, "save %i %i", &yold, &xold) == 2) {

            /*
             *
             * this is completely pointless
             * this code should get stripped
             *
             */

        } else if (strcmp(line, "clear\n") == 0) {

            /*
             *
             * i guess at some point this can be turned
             * into using the mode=2 shit
             *
             */

        } else {

            fputs("FAILURE\n", stderr);
            exit(-1);
        }
    }

    for (k = 0; k < 16; k++) {
        if (fb2ct[k][0] >= 0) {
            onblack = fb2ct[k][0];
            break;
        }
    }

    if (onblack == -1) {
        fprintf(stderr, "failed to find a black background color!\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < MAXCOLS; i++) {
        for (j = 0; j < MAXROWS; j++) {
            if (datafile[j][i] == 0)
                datafile[j][i] = /*htons */ ((onblack << 10) + ' ');
        }
    }

    packed = /*htons */ (maxypos);

    fwrite(&packed, 1, sizeof(unsigned short), stdout);

    fwrite(datafile, 1, sizeof(unsigned short) * 80 * maxypos, stdout);

    fprintf(stderr, "' ansi size : 80x%u (%u bytes) colors : %i\n", maxypos, 80 * maxypos * 2,
            font_siz);

    strcpy(cmd, "./buildpalette.pl ");

    for (i = 0; i < colortable_siz; i++) {

        snprintf(line, sizeof(line), "0x%02x%02x%02x%02x ",
                 ansi[colortable[i][0]],
                 ansi[colortable[i][1]], ansi[colortable[i][0]], ansi[colortable[i][1]]);

        strcat(cmd, line);

        /*

           fprintf(stderr, ", %%%%%s_%s_%s_%s",
           ansi[colortable[i][0]],
           ansi[colortable[i][0]],
           ansi[colortable[i][1]],
           ansi[colortable[i][1]]);

         */

        // fprintf(stderr, " ' %02i. %s on %s\n", i, ansi_name[colortable[i][0]], ansi_name[colortable[i][1]]);
    }

    strcat(cmd, "> palette.bin");

    fprintf(stderr, "\n' %s\n", cmd);

    system(cmd);

    // fprintf(stderr, "\nspacecolor WORD %i ' using the bg color entry of %i\n", onblack, onblack);

    strcpy(cmd, "./buildfontmap.pl ");

    // fputs("' ./buildfontmap.pl ", stderr);

    for (i = 0; i < font_siz; i++) {
        snprintf(line, sizeof(line), "%i ", font2byte[i]);
        strcat(cmd, line);
    }

    strcat(cmd, "2>/dev/null > font.bin");

    // fputs("2>/dev/null > font.bin\n", stderr);

    fprintf(stderr, "\n' %s\n", cmd);

    system(cmd);

    exit(EXIT_SUCCESS);
}
