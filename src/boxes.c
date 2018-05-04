#include <ncurses.h>
#include <stdlib.h>
#include "boxes.h"

void set_box_width(int screen_cols) {
  box_width = screen_cols / 4;
  padding = (screen_cols % 4) / 2;
}

void make_box_string(int length, char s[]) {
  int i;
  for (i = 0; i < length; i++) {
    s[i] = ' ';
  }
  s[i] = '\0';
}

void print_box(struct box b) {
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

void make_boxes(void) { 
  for (int i = 0; i < 3; i++) {
    boxes[i].y = (i / 4) * 5;
    boxes[i].x = (i % 4) * box_width + padding;
    boxes[i].health = rand() % (10 - 1) + 1;
  }
}

void move_boxes_down(void) {
  for (int i = 0; i < 20; i++) {
    if (boxes[i].health > 0) {
      remove_box(i);
      boxes[i].y++;
      print_box(boxes[i]);
      /*if (player_hit()) {*/
        /*gameover = 1;*/
      /*}*/
    }
  }
}


