
int box_width, padding;

struct box {
  int y;
  int x;
  int health;
};

void move_boxes_down(void);
struct box *boxes = NULL;

void set_box_width(int screen_cols);
void remove_box(int box_index);
void print_box(struct box b);
void make_box_string(int length, char s[]);
void make_boxes(void);
