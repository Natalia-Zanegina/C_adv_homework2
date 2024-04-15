#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ncurses.h>

#define MIN_Y 2
#define START_TAIL_SIZE 3
#define MAX_TAIL_SIZE 100
#define CONTROLS 3

enum {
    LEFT = 1,
    UP,
    RIGHT,
    DOWN,
    STOP_GAME = KEY_F(10)
};

struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
};

struct control_buttons default_controls[CONTROLS] = {{KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT}, {'S', 'W', 'A', 'D'}, {'s', 'w', 'a', 'd'}};

typedef struct snake_t
{
    int x;
    int y;
    int direction;
    size_t tsize;
    struct tail_t* tail;
    struct control_buttons* controls;
} snake_t;


typedef struct tail_t
{
    int x;
    int y;
} tail_t;

void initTail(tail_t t[], size_t size)
{
    tail_t init_t = {0, 0};
    for (size_t i = 0; i < size; i++)
        t[i] = init_t;
}

void initHead(snake_t* head, int x, int y)
{
    head -> x = x;
    head -> y = y;
    head -> direction = RIGHT;
}

void initSnake(snake_t* head, size_t size, int x, int y)
{
    tail_t* tail = (tail_t*) malloc(MAX_TAIL_SIZE * sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head -> tail = tail;
    head -> tsize = size + 1;
    head -> controls = default_controls;
}

void go(snake_t* head)
{
    char ch = '@';
    int max_x = 0, max_y = 0;
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(head -> y, head -> x, " ");
    switch (head -> direction)
    {
        case LEFT:
            if (head -> x <= 0)
                head -> x = max_x;
            mvprintw(head -> y, --(head -> x), "%c", ch);
            break;
        case RIGHT:
            if (head -> x >= max_x)
                head -> x = 0;
            mvprintw(head -> y, ++(head -> x), "%c", ch);
            break;
        case UP:
            if (head -> y <= MIN_Y)
                head -> y = max_y;
            mvprintw(--(head -> y), head -> x, "%c", ch);
            break;
        case DOWN:
            if (head -> y >= max_y)
                head -> y = MIN_Y;
            mvprintw(++(head -> y), head -> x, "%c", ch);
            break;
        default:
            break;
    }
    refresh();
}

void goTail(snake_t* head)
{
    char ch = '*';
    mvprintw(head -> tail[head -> tsize - 1].y,head -> tail[head -> tsize - 1].x, " ");
    for (size_t i = head -> tsize - 1; i > 0; i--)
    {
        head -> tail[i] = head -> tail[i - 1];
        if ((head -> tail[i].y) || (head -> tail[i].x))
            mvprintw(head -> tail[i].y, head -> tail[i].x, "%c", ch);
    }
    head -> tail[0].x = head -> x;
    head -> tail[0].y = head -> y;
}

int checkDirection(snake_t* snake, int32_t key)
{
    for (int i = 0; i < CONTROLS; i++)
    {    
        if ((key == snake -> controls[i].down && snake -> direction == UP) ||
        (key == snake -> controls[i].up && snake -> direction == DOWN) ||
        (key == snake -> controls[i].left && snake -> direction == RIGHT) ||
        (key == snake -> controls[i].right && snake -> direction == LEFT))
            return 0;
    }
    return 1;
}

void changeDirection(snake_t* snake, const int32_t key)
{
    if (!checkDirection(snake, key))
        return;

    for (int i = 0; i < CONTROLS; i++)
    {
        if (key == snake -> controls[i].down)
            snake -> direction = DOWN;
        else if (key == snake -> controls[i].up)
            snake -> direction = UP;
        else if (key == snake -> controls[i].right)
            snake -> direction = RIGHT;
        else if (key == snake -> controls[i].left)
            snake -> direction = LEFT;
    }
}

int main(void)
{
    snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
    initSnake(snake, START_TAIL_SIZE, 10, 10);
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    curs_set(FALSE);
    mvprintw(0, 1, "Use arrows or WASD keys for control. Press 'F10' for EXIT.");
    timeout(0);
    int key_pressed = 0;

    while (key_pressed != STOP_GAME)
    {
        key_pressed = getch();
        go(snake);
        goTail(snake);
        timeout(100);
        changeDirection(snake, key_pressed);
    }
    
    free(snake -> tail);
    free(snake);
    endwin();
    return 0;
}