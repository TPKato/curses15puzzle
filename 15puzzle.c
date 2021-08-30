#include <curses.h>
#include <sys/types.h>
#include <sys/time.h>

#ifndef SCOREFILE
#define SCOREFILE "/var/games/15puzzle.score"
#endif

#define DEFAULT_TILE_X 4
#define DEFAULT_TILE_Y 3

#define TSHUFFLE 500

#define UP	(1 << 0)
#define DOWN	(1 << 1)
#define LEFT	(1 << 2)
#define RIGHT	(1 << 3)

#define SPACE 15		/* 4 x 4 - 1 */

#define puttile(x) mvwin(tile[pos[x]], tile_y * ((x) / 4), tile_x * (int)((x) % 4));

WINDOW *tile[16];
int pos[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, SPACE};
unsigned int turn = 0;
double drand48();
int tile_x = 0, tile_y = 0, height = 0, width = 0;

const char revision[] = "$Revision: 1.2 $";

void usage()
{
  puts("15puzzle [OPTION]...\n");
  puts("  -r     reverse mode");
  printf("  -x n   tile width  (>= 2, default is %d)\n", DEFAULT_TILE_X);
  printf("  -y n   tile height (>= 1, default is %d)\n", DEFAULT_TILE_Y);
  puts("  -X n   game width  (not supported yet)");
  puts("  -Y n   game height (not supported yet)");
  puts("  -s     display score and exit");
  puts("  -h     display this help and exit");
  puts("  -v     output version information and exit");
}

void wininit()
{
  int i;

  initscr();
  cbreak();
  noecho();
  leaveok(stdscr, TRUE);

  for (i = 0; i < 15; i++) {
    tile[i] = newwin(tile_y, tile_x, tile_y * (i / 4), tile_x * (int)(i % 4));
    box(tile[i], '|', '-');
    mvwaddch(tile[i], 0, 0, '+');
    mvwaddch(tile[i], tile_y - 1, 0, '+');
    mvwaddch(tile[i], 0, tile_x - 1, '+');
    mvwaddch(tile[i], tile_y - 1, tile_x - 1, '+');
    mvwprintw(tile[i], (int)(tile_y / 2), (int)(tile_x / 2) - 1, "%2d", i + 1);
    leaveok(tile[i], TRUE);
    wrefresh(tile[i]);
  }
  tile[i] = newwin(tile_y, tile_x, tile_y * (i / 4), tile_x * (int)(i % 4));
  leaveok(tile[i], TRUE);
  wrefresh(tile[i]);
}

void movetile(int dir)
{
  int i, spc = 0, from = -1;

  for (i = 0; i < 16; i++) {
    if (pos[i] == SPACE) {
      spc = i;
    }
  }

  switch (dir) {
  case UP:
    if ((int)(spc / 4) != 3) {
      from = spc + 4;
    }
    break;
  case DOWN:
    if ((int)(spc / 4) != 0) {
      from = spc - 4;
    }
    break;
  case LEFT:
    if ((spc % 4) != 3) {
      from = spc + 1;
    }
    break;
  case RIGHT:
    if ((spc % 4) != 0) {
      from = spc - 1;
    }
    break;
  }

  if (! (from < 0)) {
    pos[spc] = pos[from];
    pos[from] = SPACE;

    puttile(spc);
    wrefresh(tile[pos[spc]]);
    puttile(from);
    wrefresh(tile[pos[from]]);
    refresh();

    turn++;
  }
}

int finished()
{
  int i, res;

  res = 1;

  for (i = 0; i < 15; i++) {
    if (pos[i] != i) {
      res &= 0;
    }
  }

  return res;
}

void shuffle()
{
  int i, dir;

  for (i = 0; i < TSHUFFLE; i++) {
    dir = (int)(drand48() * 4);
    movetile(1<<dir);
  }
}

void showscore()
{
  puts(" Name                    Times    Turns");
  puts("----------------------------------------");
  system("sort -n +2 -t: " SCOREFILE " | head -10 | nawk '{split($0, a, \":\") ; printf(\" %-20s %8d %8d\\n\", a[1], a[2], a[3])}'");
  puts("----------------------------------------");
}

void score(long time)
{
  char name[256], *i;
  FILE *fp;
  WINDOW *input;

  input = newwin(0, 0, tile_y * 4, 0);
  /* --- 4 -> height */
  scrollok(input, TRUE);
  echo();

  wprintw(input, "Finished! (%ld sec, %d turns)\n", time, turn);

  waddstr(input, "Your name (less than 20chars)\nvvvvvvvvvvvvvvvvvvvv\n");
  wrefresh(input);
  wgetstr(input, name);

  name[20] = '\0';

  /* translate ':' -> ' ' */
  i = name;
  while (*i) {
    if (*i == ':') *i = ' ';
    i++;
  }

  if((fp = fopen(SCOREFILE, "a")) == NULL) {
    perror(SCOREFILE);
    exit(1);
  }
  fprintf(fp, "%s:%ld:%d\n", name, time, turn);
  fclose(fp);

  endwin();
  showscore();
}

int main(int argc, char **argv)
{
  time_t start, end;
  int rev = 0, c;
  extern char *optarg;
  extern int optind;

  /* arguments */
  while ((c = getopt(argc, argv, "rx:y:X:Y:hvs")) != -1) {
    switch (c) {
    case 'r':
      rev = (UP | DOWN | LEFT | RIGHT);
      break;
    case 'x':
      tile_x = atoi(optarg);
      break;
    case 'y':
      tile_y = atoi(optarg);
      break;
    case 'X':
      height = atoi(optarg);
      break;
    case 'Y':
      width = atoi(optarg);
      break;
    case 'h':
      usage();
      exit(1);
      break;
    case 'v':
      fprintf(stderr, "15puzzle: %s\n", revision);
      exit(1);
      break;
    case 's':
      showscore();
      exit(1);
      break;
    case '?':
      puts("Try `15puzzle -h' for more information.");
      exit(1);
      break;
    }
  }

  if (! tile_x) tile_x = DEFAULT_TILE_X;
  if (! tile_y) tile_y = DEFAULT_TILE_Y;

  srand48((long)time(NULL));
  wininit();
  shuffle();

  (void)time(&start);

  do {
    switch (getch()) {
    case 'k': movetile(UP    ^ ((UP | DOWN) & rev)); break;
    case 'j': movetile(DOWN  ^ ((UP | DOWN) & rev)); break;
    case 'h': movetile(LEFT  ^ ((LEFT | RIGHT) & rev)); break;
    case 'l': movetile(RIGHT ^ ((LEFT | RIGHT) & rev)); break;
    }
  } while (! finished());

  (void)time(&end);

  score((long)(end - start));

  return 0;
}
