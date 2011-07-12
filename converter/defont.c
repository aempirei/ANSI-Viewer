#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

int
main(int argc, char **argv)
{

   int width, height;
   int n;
   int dx, dy, x, y;
   int i, j, ch;
   int byte;

   int xo[] = {
      0, 48, 96, 144,
      192, 240, 288, 336,
      384, 432, 480, 528,
      576, 624, 672, 720
   };

   unsigned long offset;
   unsigned long value;

   unsigned char *data;

   char line[128];

   if (argc != 2) {
      fprintf(stderr, "\nusage: %s char_no\n\n", *argv);
      exit(EXIT_SUCCESS);
   }

   byte = atoi(argv[1]);

   fgets(line, sizeof(line) - 1, stdin);

   assert(strcmp(line, "P5\n") == 0);

   n = scanf("%i %i\n", &width, &height);

   assert(n == 2);

   dx = width / 16;
   dy = height / 16;

   assert(width % 16 == 0);
   assert(height % 16 == 0);

   n = scanf("%i\n", &i);

   assert(i == 255);

   fprintf(stderr, "%i x %i : step %i x %i\n", width, height, dx, dy);

   data = malloc(width * height);

   assert(data != NULL);

   n = fread(data, width * height, 1, stdin);

   assert(n == 1);

   assert(sizeof(value) == 4);

   ch = byte;

   i = ch % 16;
   j = ch / 16;

   x = xo[i];
   y = j * dy;

   // printf("' $%02x : %i <%i %i> <%i %i>\n", ch, ch, i, j, x, y);

   for (j = 0; j < 32; j++) {

      // fputs("long %%", stdout);

      value = 0;

      for (i = 0; i < 16; i++) {

         offset = (x + i) + ((y + j) * width);

         assert(offset < width * height);

         n = data[offset];

         // putchar(n == 0 ? '1' : '0');
         //
         value |= (n == 0 ? 1 : 0) << (i * 2);
      }

      fwrite(&value, sizeof(value), 1, stdout);

      // putchar('\n');
   }

   exit(EXIT_SUCCESS);
}
