#define LEFT 1
#define RIGHT 2

void update();
void print_game();
void print_gameover();
void new_game();
void handle_input(int c);
void move_player(int direction);
void fire_projectile(void);

int player_pos;
int mouse_pos;
int fire_rate = 1;
int fire_power = 1;
int paused = 0;
int gameover = 0;
int running = 1;
int score;
int screen_rows, screen_cols;
struct point *projectiles = NULL;
struct box *boxes = NULL;

struct point {
  int y;
  int x;
};

struct box {
  int y;
  int x;
  int health;
};

int projectile_hit(struct point p);

MEVENT event;

