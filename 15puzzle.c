#include <curses.h>
#include <sys/types.h>
#include <sys/time.h>

#define TILE_X 5
#define TILE_Y 3
#define TSHUFFLE 500

#define UP	(1 << 0)
#define DOWN	(1 << 1)
#define LEFT	(1 << 2)
#define RIGHT	(1 << 3)

#define SPACE 15

#define puttile(x) mvwin(tile[pos[x]], TILE_Y * ((x) / 4), TILE_X * (int)((x) % 4));

WINDOW *tile[16];
int pos[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, SPACE};
unsigned int turn = 0;
double drand48();
char scorefile[] = "15puzzle.score";

void wininit()
{
  int i;

  initscr();
  cbreak();
  noecho();
  leaveok(stdscr, TRUE);

  for (i = 0; i < 15; i++) {
    tile[i] = newwin(TILE_Y, TILE_X, TILE_Y * (i / 4), TILE_X * (int)(i % 4));
    box(tile[i], '|', '-');
    mvwaddch(tile[i], 0, 0, '+');
    mvwaddch(tile[i], TILE_Y - 1, 0, '+');
    mvwaddch(tile[i], 0, TILE_X - 1, '+');
    mvwaddch(tile[i], TILE_Y - 1, TILE_X - 1, '+');
    mvwprintw(tile[i], 1, 1, "%2d", i + 1);
    leaveok(tile[i], TRUE);
    wrefresh(tile[i]);
  }
  tile[i] = newwin(TILE_Y, TILE_X, TILE_Y * (i / 4), TILE_X * (int)(i % 4));
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

void score(long time)
{
  char name[256];
  FILE *fp;

  printf("Finished! (%ld sec, %d turns)\n", time, turn);

  puts("Your name (less than 16chars)\nvvvvvvvvvvvvvvv");
  gets(name);

  if((fp = fopen(scorefile, "a")) == NULL) {
    perror(scorefile);
    exit(1);
  }

  fprintf(fp, "%-16s %8ld %8d\n", name, time, turn);
}

int main(int argc, char **argv)
{
  time_t start, end;
  int rev = 0;

  if (argc > 1) {
    if (strncmp(argv[1], "-rev", 4)) {
      rev = 0;
    } else {
      rev = (UP | DOWN | LEFT | RIGHT);
    }
  }

  srand48((long)time(NULL));
  wininit();
  shuffle();

  printf(">%d<", rev);

  (void)time(&start);

  do {
    switch (getch()) {
    case 'k':
      if (rev) movetile(DOWN); else movetile(UP); break;
    case 'j':
      if (rev) movetile(UP); else movetile(DOWN); break;
    case 'h':
      if (rev) movetile(RIGHT); else movetile(LEFT); break;
    case 'l':
      if (rev) movetile(LEFT); else movetile(RIGHT); break;
    }
  } while (! finished());

  (void)time(&end);

  endwin();
  score((long)(end - start));

  return 0;
}
