#define LEFT 1
#define RIGHT 2

void update(void);
void print_game(void);
void print_gameover(void);
void new_game(void);
void handle_input(int c);
void move_player(int direction);
void fire_projectile(void);
void move_projectiles_up(void);

int player_pos;
int mouse_pos = -1;
int fire_rate = 1;
int fire_power = 1;
int paused = 0;
int gameover = 0;
int running = 1;
int box_tick = 0;
int proj_tick = 0;
int score;
int screen_rows, screen_cols;
struct point *projectiles = NULL;

struct point {
  int y;
  int x;
};

int projectile_hit(struct point p);

MEVENT event;

