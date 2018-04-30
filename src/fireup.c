#include <ncurses.h>
// time(), nanosleep()
#include <time.h>
// srand(), rand(), malloc()
#include <stdlib.h>
#include "fireup.h"

void make_box_string(int length, char s[]);
int player_hit(void);
void make_boxes(void);

int main(void) {
  struct timespec ts = {0, 10000000L};
  int c;

  srand(time(NULL));

  initscr();

  // Hide the cursor
  curs_set(0);

  // Don't show keypress on screen
  cbreak();
  noecho();

  // Enable colours
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_YELLOW); // Projectile
  init_pair(2, COLOR_BLUE, COLOR_BLUE); // Player
  init_pair(3, COLOR_BLACK, COLOR_BLACK); // Background

  init_pair(4, COLOR_WHITE, COLOR_GREEN); // Green Block
  init_pair(5, COLOR_WHITE, COLOR_YELLOW); // Yellow Block
  init_pair(6, COLOR_WHITE, COLOR_RED); // Red Block

  // Stop getch() from blocking
  nodelay(stdscr, TRUE);

  keypad(stdscr, TRUE);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

  getmaxyx(stdscr, screen_rows, screen_cols);

  new_game();

  while (running) {
    while ((c = getch()) != ERR)
      handle_input(c);

    if (!paused && !gameover) {
      update();
      print_game();
    }

    if (gameover)
      print_gameover();

    refresh();
    nanosleep(&ts, NULL);
  }

  // Clean up after ourselves
  free(projectiles);
  endwin();

  return 0;
}

void handle_input(int c) {
  switch (c) {
  case KEY_MOUSE:
    getmouse(&event);
    // TERM=xterm-1003 is required for mouse movement
    // https://stackoverflow.com/questions/7462850/mouse-movement-events-in-ncurses
    mouse_pos = event.x;
    break;
  case 'a':
    move_player(LEFT);
    break;
  case 'd':
    move_player(RIGHT);
    break;
  case ' ':
    // toggle paused
    paused ^= 1;
    if (paused)
      mvprintw(0, screen_cols / 2 - 3, "Paused");
    break;
  case 'q':
    running = 0;
    break;
  case 'n':
    new_game();
    break;
  }
}

void update(void) {
  if (box_tick++ == 10) {
    move_boxes_down();
    box_tick = 0;
  }
  if (proj_tick++ == 15) {
    fire_projectile();
    proj_tick = 0;
  }

  move_projectiles_up();

  if (mouse_pos < player_pos && mouse_pos != -1) {
    move_player(LEFT);
  } else if (mouse_pos > player_pos) {
    move_player(RIGHT);
  }
}

void fire_projectile(void) {
  for (int i = 0; i < screen_rows; i++) {
    if (projectiles[i].y < 0) {
      projectiles[i].y = screen_rows - 2;
      projectiles[i].x = player_pos + 1;
      attron(COLOR_PAIR(1));
      mvprintw(projectiles[i].y, projectiles[i].x, " ");
      attroff(COLOR_PAIR(1));
      return;
    }
  }
}

void move_projectiles_up(void) {
  for (int i = 0; i < screen_rows; i++) {
    mvprintw(projectiles[i].y, projectiles[i].x, " ");

    if (projectile_hit(projectiles[i]) || projectiles[i].y-- < 0) {
      projectiles[i].y = -1;
      projectiles[i].x = -1;
    }

    attron(COLOR_PAIR(1));
    mvprintw(projectiles[i].y, projectiles[i].x, " ");
    attroff(COLOR_PAIR(1));
  }

}

int projectile_hit(struct point p) {
  int box_width = (screen_cols / 4) - 1;

  for (int i = 0; i < 20; i++) {
    if (p.y > boxes[i].y && p.y < boxes[i].y + 2 &&
        p.x > boxes[i].x - 1 && p.x < boxes[i].x + box_width) {
      boxes[i].health--;
      score++;

      if (boxes[i].health < 1) {
        remove_box(i);
      } else {
        print_box(boxes[i]);
      }
      return 1;
    }
  }

  return 0;
}

void print_box(struct box b) {
  int box_width = (screen_cols / 4) - 1;
  char s[box_width+1];

  make_box_string(box_width, s);

  if (b.health < 4) {
    attron(COLOR_PAIR(4));
  } else if (b.health < 6) {
    attron(COLOR_PAIR(5));
  } else {
    attron(COLOR_PAIR(6));
  }
  mvprintw(b.y, b.x, "%s", s);
  mvprintw(b.y - 1, b.x, "%s", s);
  mvprintw(b.y - 1, b.x + (box_width/2), "%d", b.health);
  mvprintw(b.y - 2, b.x, "%s", s);
  if (b.health < 4) {
    attroff(COLOR_PAIR(4));
  } else if (b.health < 6) {
    attroff(COLOR_PAIR(5));
  } else {
    attroff(COLOR_PAIR(6));
  }
}

void remove_box(int bi) {
  int box_width = (screen_cols / 4) - 1;
  char s[box_width+1];

  make_box_string(box_width, s);

  mvprintw(boxes[bi].y, boxes[bi].x, "%s", s);
  mvprintw(boxes[bi].y - 1, boxes[bi].x, "%s", s);
  mvprintw(boxes[bi].y - 2, boxes[bi].x, "%s", s);
  if (boxes[bi].health < 1) {
    boxes[bi].x = -1;
    boxes[bi].y = -1;
  }
}

void print_game(void) {
  mvprintw(0, 3, "Score: %d", score);

  attron(COLOR_PAIR(2));
  mvprintw(screen_rows - 2, player_pos, "   ");
  attroff(COLOR_PAIR(2));
}

void print_gameover(void) {
  mvprintw(screen_rows / 2, screen_cols / 2 - 5, "Game Over!");
  mvprintw(screen_rows / 2 + 1, screen_cols / 2 - 15,
    "Press 'n' to start a new game!");
}

void new_game(void) {
  clear();

  player_pos = screen_cols / 2;

  if (!projectiles) {
    projectiles = malloc(screen_rows * sizeof *projectiles);
  }
  if (!boxes) {
    boxes = malloc(20 * sizeof *boxes);
  }

  for (int i = 0; i < screen_rows; i++) {
    projectiles[i].y = -1;
    projectiles[i].x = -1;
  }
  for (int i = 0; i < 20; i++) {
    boxes[i].y = -1;
    boxes[i].x = -1;
    boxes[i].health = -1;
  }

  //boxes[0].y = 0;
  //boxes[0].x = 0;
  //boxes[0].health = 5;//rand() % (10 - 1) + 1;

  //boxes[1].y = 0;
  //boxes[1].x = 1 * (screen_cols / 4);
  //boxes[1].health = 5;//rand() % (10 - 1) + 1;
 
  //boxes[2].y = 0;
  //boxes[2].x = 2 * (screen_cols / 4);
  //boxes[2].health = 5;//rand() % (10 - 1) + 1;

  //boxes[3].y = 0;
  //boxes[3].x = 3 * (screen_cols / 4);
  //boxes[3].health = 5;//rand() % (10 - 1) + 1;
  make_boxes();

  for (int i = 0; i < 20; i++) {
    if (boxes[i].health > 0)
      print_box(boxes[i]);
  }
   
  score = 0;
  gameover = 0;
  paused = 0;
}

void make_boxes(void) {
  for (int i = 0; i < screen_cols / 3; i++) {
    boxes[i].y = i * 3;
    boxes[i].x = (i % 4) * (screen_cols / 4);
    boxes[i].health = rand() % (10 - 1) + 1;
  }
}

void move_boxes_down(void) {
  for (int i = 0; i < 20; i++) {
    if (boxes[i].health > 0) {
      remove_box(i);
      boxes[i].y++;
      print_box(boxes[i]);
      if (player_hit()) {
        gameover = 1;
      }
    }
  }
}

int player_hit(void) {
  int box_width = (screen_cols / 4) - 1;

  for (int i = 0; i < 20; i++) {
    if (boxes[i].y == screen_rows - 2 &&
        player_pos > boxes[i].x - 1 && player_pos < boxes[i].x + box_width) {
      return 1;
    }
  }

  return 0;
}

void move_player(int direction) {
  switch (direction) {
  case LEFT:
    if (player_pos != 0)
      player_pos--;
    break;
  case RIGHT:
    if (player_pos != screen_cols - 3)
      player_pos++;
    break;
  }
  move(screen_rows - 2, 0);
  clrtoeol();
}

void make_box_string(int length, char s[]) {
  int i;
  for (i = 0; i < length; i++) {
    s[i] = ' ';
  }
  s[i] = '\0';
}
