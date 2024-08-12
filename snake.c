#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
// declare a struct
struct SNAKE
{
    int x;
    int y;
    int last_x;
    int last_y;
    struct SNAKE *next;
};
struct MODE
{
    int brick;
    int swirl;
    int wormhole;
    int easy;
    int mix;
    int sum;
} mode = {};
// declare variables
HWND console;
HANDLE handle;
CONSOLE_SCREEN_BUFFER_INFO csbi;
int maxwid, maxhet; // for board
int clickarrow;
int point, point_x, point_y;
int ori_foreground, ori_background;
int speed; // snake's moving speed
int step;  // snake's moved steps
int safe;  // 0 is safe, 1 is unsafe
int shuffle;
// declare functions
struct SNAKE *initsnake(); // initialize a node
void addtosnake();         // add a node to head node
void setconsole();         // default screen and max board
void setmode();            // choose mode to play
void setfence();           // print the board
void seteffect();          // set effect into game
void validpos();           // for seteffect(), check if set repeatedly
void printeffect();        // print effect
void doeffect();           // deal with the react between snake and effect
void easymode();           // for EASY mode
void cleareffect();        // clear inner-board effect
void freelist();           // for cleareffect(), free the linked list
void statebar();           // show current state
void setpoint();           // set and print point into game
void colorprint();         // print string with color
void printsnake();         // print snake
void claertail();          // for move snake, clear snake's tail
void input();              // deal with kb input
void trackhead();          // make each node track to last one
void ch_speed();           // change speed
void fail();               // detect if fail
void end();                // do system for fail
void endcartoon();         // for end(), ending cartoon
// main function
int main()
{
    // default datas
    console = GetConsoleWindow();
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    maxwid = 0, maxhet = 0;
    srand(time(NULL));
    clickarrow = 0;
    point = 0, point_x = 0, point_y = 0;
    ori_foreground = 0;                    // black
    ori_background = BACKGROUND_INTENSITY; // gray
    speed = 50;
    step = 0;
    safe = 0;
    shuffle = 0;
    setconsole();
    setmode();
    struct SNAKE *snakehead = initsnake(maxwid / 2, maxhet / 2); // default the snake
    struct SNAKE *brick = initsnake(0, 0);                       // default brick
    struct SNAKE *swirl = initsnake(0, 0);                       // default swirl
    struct SNAKE *wormhole = initsnake(0, 0);                    // default wormhole
    // start gaming
    while (safe == 0)
    {
        setfence();
        seteffect(snakehead, brick, swirl, wormhole);
        setpoint(snakehead, brick, swirl, wormhole);
        statebar();
        input(snakehead);
        ch_speed(handle);
        printeffect(brick, swirl, wormhole);
        printsnake(snakehead);
        claertail(snakehead);
        doeffect(&snakehead, swirl, wormhole);
        fail(snakehead, brick);
        easymode(snakehead);
        cleareffect(&brick, &swirl, &wormhole);
        Sleep(speed);
        end(&snakehead, &brick, &swirl, &wormhole);
    }
}
struct SNAKE *initsnake(int x, int y)
{
    struct SNAKE *snake = (struct SNAKE *)malloc(sizeof(struct SNAKE));
    snake->x = x;
    snake->y = y;
    snake->last_x = x;
    snake->last_y = y;
    snake->next = NULL;
    return snake;
}
void addtosnake(struct SNAKE *head, int x, int y)
{
    struct SNAKE *newtail = initsnake(x, y);
    struct SNAKE *current = head;
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = newtail;
}
void setconsole()
{
    ShowWindow(console, SW_MAXIMIZE);
    GetConsoleScreenBufferInfo(handle, &csbi);
    maxwid = 2 * csbi.srWindow.Bottom; // set max width
    maxhet = csbi.srWindow.Bottom;     // set max height
    SetConsoleTitleW(L"SNAKE");        // LPCWSTR
    system("color 80");
}
void setmode()
{
    mode.brick = 0;
    mode.swirl = 0;
    mode.wormhole = 0;
    mode.easy = 0;
    mode.mix = 0;
    mode.sum = 0;
    setfence();
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(handle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(handle, &cursorInfo); // set cursor unvisible
    int arrow;
    COORD crdmode;
    crdmode.X = maxwid + 2;
    crdmode.Y = 1;
    colorprint(crdmode, "CHOOSE--MODE", FOREGROUND_GREEN, 0); // fore: green, back: black
    crdmode.Y = 2;
    colorprint(crdmode, "| NORMAL   |", 1, 0); // fore: blue, back: black
    crdmode.Y = 3;
    colorprint(crdmode, "| SPECIAL v|", 1, 0); // fore: blue, back: black
    crdmode.Y = 11;
    colorprint(crdmode, "#", FOREGROUND_RED | FOREGROUND_INTENSITY, 0); // fore: light red, back: black
    printf(" -> BRICK");
    crdmode.Y = 13;
    colorprint(crdmode, "@", 7, BACKGROUND_BLUE | BACKGROUND_INTENSITY); // fore: white, back: light blue
    printf(" -> SWIRL");
    crdmode.Y = 15;
    colorprint(crdmode, "&", 9, BACKGROUND_RED | BACKGROUND_BLUE); // fore: light blue, back: purple
    printf(" -> WORMHOLE");
    crdmode.Y = 2;
    while (1)
    {
        if (crdmode.Y > 2) // open SPECIAL list
        {
            COORD crdspecial;
            crdspecial.X = crdmode.X;
            crdspecial.Y = 4;
            colorprint(crdspecial, " - BRICK   |", 1, 0); // fore: blue, back: black
            crdspecial.Y = 5;
            colorprint(crdspecial, " - SWIRL   |", 1, 0); // fore: blue, back: black
            crdspecial.Y = 6;
            colorprint(crdspecial, " - WORMHOLE|", 1, 0); // fore: blue, back: black
            crdspecial.Y = 7;
            colorprint(crdspecial, " - EASY    |", 1, 0); // fore: blue, back: black
            crdspecial.Y = 8;
            colorprint(crdspecial, " - MIX     |", 1, 0); // fore: blue, back: black
        }
        else // clear the list
        {
            int n;
            for (n = 4; n <= 8; n++)
            {
                COORD crdspecial;
                crdspecial.X = crdmode.X;
                crdspecial.Y = n;
                colorprint(crdspecial, "            ", ori_foreground, ori_background); // fore: black, back: gray
            }
        }
        if (_kbhit()) // get a arrow
        {
            arrow = getch();
            if (arrow == 224)
            {
                arrow = getch();
                if (arrow == 72 || arrow == 80) // up or down
                {
                    switch (arrow) // do the arrow
                    {
                    case 72: // up
                        if (crdmode.Y == 2)
                            crdmode.Y = 2;
                        else
                            crdmode.Y--;
                        break;
                    case 80: // down
                        if (crdmode.Y == 8)
                            crdmode.Y = 8;
                        else
                            crdmode.Y++;
                        break;
                    }
                }
            }
            else if (arrow == 13) // enter
            {
                if (crdmode.Y == 3) // enter on SPECIAL
                    continue;
                else if (crdmode.Y == 4) // brick
                    mode.brick = 1;
                else if (crdmode.Y == 5) // swirl
                    mode.swirl = 1;
                else if (crdmode.Y == 6) // wormhole
                    mode.wormhole = 1;
                else if (crdmode.Y == 7) // easy
                    mode.easy = 1;
                else if (crdmode.Y == 8) // mix
                    mode.mix = 1;
                crdmode.X += 14;
                colorprint(crdmode, "<<", 7, ori_background); // fore: white, back: gray
                mode.sum = mode.brick + mode.swirl + mode.wormhole + mode.easy + mode.mix;
                return;
            }
        }
        COORD crdblink;
        crdblink.X = crdmode.X + 14;
        crdblink.Y = crdmode.Y;
        colorprint(crdblink, "<<", 7, ori_background); // fore: white, back: gray
        Sleep(100);
        colorprint(crdblink, "  ", ori_foreground, ori_background); // fore: black, back: gray
        Sleep(100);
        COORD crd00 = {0, 0};
        SetConsoleCursorPosition(handle, crd00);
    }
}
void setfence()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(handle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(handle, &cursorInfo); // set cursor unvisible
    COORD crdfence;
    crdfence.X = 0;
    crdfence.Y = 0;
    for (crdfence.X = 0; crdfence.X <= maxwid; crdfence.X++)
    {
        for (crdfence.Y = 0; crdfence.Y <= maxhet; crdfence.Y++) // print the fence
        {
            if (crdfence.Y == 0 || crdfence.Y == maxhet ||
                crdfence.X == 0 || crdfence.X == maxwid)
            {
                colorprint(crdfence, "#", FOREGROUND_RED | FOREGROUND_INTENSITY, 0); // fore: light red, back: black
            }
        }
    }
    crdfence.X = 0;
    crdfence.Y = 0;
    SetConsoleCursorPosition(handle, crdfence); // fixed the windiw
}
void seteffect(struct SNAKE *head, struct SNAKE *brick,
               struct SNAKE *swirl, struct SNAKE *wormhole)
{
    int n = 0;
    int modearr[] = {mode.brick, mode.swirl, mode.wormhole, mode.mix};
    struct SNAKE *arr[] = {brick, swirl, wormhole};
    for (n = 0; n < 3 && step % 51 == 50; n++)
    {
        if (modearr[n] == 1 || modearr[3] == 1)
        {
            int rd_x = 0, rd_y = 0;
            int num = rand() % maxhet / 5 + 5;
            while (num > 0)
            {
                validpos(head, brick, swirl, wormhole, &rd_x, &rd_y);
                addtosnake(arr[n], rd_x, rd_y);
                num--;
            }
        }
    }
}
void validpos(struct SNAKE *head, struct SNAKE *brick, struct SNAKE *swirl,
              struct SNAKE *wormhole, int *rdx, int *rdy)
{
    int rd_x = 0, rd_y = 0;
    int n = 0;
    struct SNAKE *arr[] = {head, brick, swirl, wormhole};
    for (n = 0; n < 4; n++)
    {
        while (arr[n] != NULL)
        {
            if ((rd_x == arr[n]->x && rd_y == arr[n]->y) || (rd_x == point_x && rd_y == point_y) ||
                (rd_x == 0 || rd_y == 0 || rd_x == maxwid || rd_y == maxhet)) // avoid bug
            {
                rd_x = rand() % maxwid;
                rd_y = rand() % maxhet;
                continue;
            }
            arr[n] = arr[n]->next;
        }
    }
    *rdx = rd_x;
    *rdy = rd_y;
}
void printeffect(struct SNAKE *brick, struct SNAKE *swirl,
                 struct SNAKE *wormhole)
{
    int n = 0;
    int modearr[] = {mode.brick, mode.swirl, mode.wormhole, mode.mix};
    struct SNAKE *arr[] = {brick, swirl, wormhole};
    for (n = 0; n < 3; n++)
    {
        if (modearr[n] == 1 || modearr[3] == 1)
        {
            while (arr[n] != NULL) // print effect
            {
                COORD crd;
                crd.X = arr[n]->x;
                crd.Y = arr[n]->y;
                if (crd.X != 0 || crd.Y != 0)
                {
                    if (n == 0)                                                          // print brick
                        colorprint(crd, "#", FOREGROUND_RED | FOREGROUND_INTENSITY, 0);  // fore: light red, back: black
                    else if (n == 1)                                                     // print swirl
                        colorprint(crd, "@", 7, BACKGROUND_BLUE | BACKGROUND_INTENSITY); // fore: white, back: light blue
                    else if (n == 2)                                                     // print wormhole
                        colorprint(crd, "&", 9, BACKGROUND_RED | BACKGROUND_BLUE);       // fore: light blue, back: purple
                }
                arr[n] = arr[n]->next;
            }
        }
    }
}
void doeffect(struct SNAKE **head, struct SNAKE *swirl,
              struct SNAKE *wormhole)
{
    struct SNAKE *cur_effect = swirl;
    if (mode.swirl == 1 || mode.mix == 1) // swirl effect
    {
        struct SNAKE *current = *head;
        while (cur_effect != NULL)
        {
            if ((*head)->x == cur_effect->x && (*head)->y == cur_effect->y)
            {
                if ((*head)->next == NULL || point == 0)
                {
                    safe = 1; // only one node then fail or no point
                    return;
                }
                while (current->next != NULL) // decrease one node and one point
                {
                    if (current->next->next == NULL) // find tail
                    {
                        COORD crdswirl;
                        crdswirl.X = current->next->x;
                        crdswirl.Y = current->next->y;
                        colorprint(crdswirl, " ", ori_foreground, ori_background);
                        // fore: black, back: gray, clear tail track
                        free(current->next);
                        current->next = NULL;
                        point--;
                        break;
                    }
                    current = current->next;
                }
                cur_effect->x = 0;
                cur_effect->y = 0;
                break;
            }
            cur_effect = cur_effect->next;
        }
    }
    if (mode.wormhole == 1 || mode.mix == 1) // wormhole effect
    {
        cur_effect = wormhole;
        while (cur_effect != NULL)
        {
            if ((*head)->x == cur_effect->x && (*head)->y == cur_effect->y)
            {
                if (cur_effect->next != NULL) // transmit to next node
                {
                    (*head)->x = cur_effect->next->x;
                    (*head)->y = cur_effect->next->y;
                }
                else if (cur_effect->next == NULL) // go back to effect's head
                {
                    (*head)->x = wormhole->x;
                    (*head)->y = wormhole->y;
                }
                break;
            }
            cur_effect = cur_effect->next;
        }
    }
}
void easymode(struct SNAKE *head)
{
    if ((mode.easy == 1 || mode.mix == 1) && safe == 1 && point > 4) // pay 5 points for more life
    {
        point -= 5;
        safe = 0;
        head->x = maxwid / 2; // transmit back to middle of board
        head->y = maxhet / 2;
    }
}
void cleareffect(struct SNAKE **brick, struct SNAKE **swirl,
                 struct SNAKE **wormhole)
{
    int n = 0;
    int reset = 0;
    int modearr[] = {mode.brick, mode.swirl, mode.wormhole, mode.mix};
    struct SNAKE **arr[] = {brick, swirl, wormhole};
    for (n = 0; n < 3 && step % 51 == 50; n++)
    {
        if (modearr[n] == 1 || modearr[3] == 1)
        {
            while ((*arr[n]) != NULL)
            {
                COORD crd;
                crd.X = (*arr[n])->x;
                crd.Y = (*arr[n])->y;
                colorprint(crd, " ", ori_foreground, ori_background); // fore: black, back: gray
                (*arr[n]) = (*arr[n])->next;
            }
            freelist(arr[n], 0, 0);
            reset = 1;
        }
    }
    shuffle += reset; // effect's position changed
}
void freelist(struct SNAKE **onestruct, int initx, int inity)
{
    struct SNAKE *current = *onestruct;
    while (current != NULL) // free all nodes
    {
        struct SNAKE *temp = current;
        current = current->next;
        free(temp);
    }
    *onestruct = initsnake(initx, inity); // initialize original pointer
}
void statebar()
{
    COORD crdstate;
    crdstate.X = maxwid + 2;
    crdstate.Y = 17;
    colorprint(crdstate, "CURRENT SPEED:", ori_foreground,
               BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE); // fore: black, back: white
    printf(" %d ", speed);                                           // show snake's speed
    crdstate.Y = 19;
    colorprint(crdstate, "TOTAL STEPS:", ori_foreground,
               BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE); // fore: black, back: white
    printf(" %d", step);                                             // show total steps
    crdstate.Y = 21;
    colorprint(crdstate, "EFFECT'S POSITION SHUFFLED TIMES:", ori_foreground,
               BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE); // fore: black, back: white
    printf(" %d", shuffle);                                          // show shuffled times
    FILE *file = fopen("highest score.txt", "r+");
    COORD crdscore;
    crdscore.X = maxwid + 2;
    crdscore.Y = 23;
    colorprint(crdscore, "CURRENT SCORE:", ori_foreground,
               BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE); // fore: black, back: white
    printf(" %d    ", point);                                        // show current score
    int hstscore;
    fscanf(file, "%d", &hstscore);
    if (point > hstscore) // write the new highest score into file
    {
        hstscore = point;
        fseek(file, 0, SEEK_SET);
        fprintf(file, "%d", point);
    }
    crdscore.Y = 25;
    colorprint(crdscore, "THE HIGHEST SCORE:", ori_foreground,
               BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE); // fore: black, back: white
    printf(" %d", hstscore);                                         // show highest score
    fclose(file);
}
void setpoint(struct SNAKE *head, struct SNAKE *brick,
              struct SNAKE *swirl, struct SNAKE *wormhole)
{
    if (point_x == 0 && point_y == 0)
    {
        int rd_x = 0, rd_y = 0;
        validpos(head, brick, swirl, wormhole, &rd_x, &rd_y);
        COORD crdpoint;
        crdpoint.X = rd_x;
        crdpoint.Y = rd_y;
        point_x = rd_x;                                                  // set x for point
        point_y = rd_y;                                                  // set y for point
        colorprint(crdpoint, "$", 0, BACKGROUND_RED | BACKGROUND_GREEN); // fore: black, back: yellow
    }
}
void colorprint(COORD crd, const char *str,
                int foreground, int background)
{
    SetConsoleCursorPosition(handle, crd);
    SetConsoleTextAttribute(handle, foreground | background);
    printf("%s", str);
    SetConsoleTextAttribute(handle, ori_foreground | ori_background);
}
void printsnake(struct SNAKE *head)
{
    struct SNAKE *current = head->next;
    COORD crdsnake;
    crdsnake.X = head->x;
    crdsnake.Y = head->y;
    colorprint(crdsnake, "O", 0, ori_background); // fore: black, back: gray
    // print snake head, fore: light green, back: black
    while (current != NULL) // print snake body
    {
        if (current->next == NULL)
        {
            crdsnake.X = current->x;
            crdsnake.Y = current->y;
            colorprint(crdsnake, "#", 0, ori_background); // fore: black, back: gray
            // print snake tail, fore: light green, back: black
            break;
        }
        else
        {
            crdsnake.X = current->x;
            crdsnake.Y = current->y;
            colorprint(crdsnake, "+", 0, ori_background); // fore: black, back: gray
            // print snake body, fore: light green, back: black
            current = current->next;
        }
    }
}
void claertail(struct SNAKE *head)
{
    if (step > 0)
    {
        struct SNAKE *current = head;
        while (current->next != NULL) // find snake tail
        {
            current = current->next;
        }
        if (current->x == point_x && current->y == point_y) // if snakehead get point, add a node to last tail
        {
            addtosnake(head, current->last_x, current->last_y);
            point_x = 0; // initialize the point position
            point_y = 0;
            point++;
        }
        COORD crdsnake;
        crdsnake.X = current->last_x;
        crdsnake.Y = current->last_y;
        colorprint(crdsnake, " ", ori_foreground, ori_background); // fore: black, back: gray
    }
}
void input(struct SNAKE *head)
{
    int last_arrow = clickarrow; // record the last clickarrow
    if ((GetAsyncKeyState(VK_UP) & 0x8000) &&
        (GetAsyncKeyState(VK_RIGHT) & 0x8000) && mode.sum > 0) // right & up
    {
        clickarrow = 149;
    }
    else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) &&
             (GetAsyncKeyState(VK_RIGHT) & 0x8000) && mode.sum > 0) // right & down
    {
        clickarrow = 157;
    }
    else if ((GetAsyncKeyState(VK_UP) & 0x8000) &&
             (GetAsyncKeyState(VK_LEFT) & 0x8000) && mode.sum > 0) // left & up
    {
        clickarrow = 147;
    }
    else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) &&
             (GetAsyncKeyState(VK_LEFT) & 0x8000) && mode.sum > 0) // left & down
    {
        clickarrow = 155;
    }
    else if (GetAsyncKeyState(VK_UP) & 0x8000) // up
    {
        clickarrow = 72;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) // down
    {
        clickarrow = 80;
    }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) // left
    {
        clickarrow = 75;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) // right
    {
        clickarrow = 77;
    }
    else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) // esc
    {
        safe = 1;
        return;
    }
    if ((clickarrow == 72 && last_arrow == 80) || (clickarrow == 80 && last_arrow == 72) ||
        (clickarrow == 75 && last_arrow == 77) || (clickarrow == 77 && last_arrow == 75) ||
        (clickarrow == 149 && last_arrow == 155) || (clickarrow == 155 && last_arrow == 149) ||
        (clickarrow == 147 && last_arrow == 157) || (clickarrow == 157 && last_arrow == 147)) // prevent from inputing opposite arrow
    {
        clickarrow = last_arrow;
    }
    int movex = 0, movey = 0;
    switch (clickarrow)
    {
    case 72: // up
        movey = -1;
        break;
    case 80: // down
        movey = 1;
        break;
    case 75: // left
        movex = -1;
        break;
    case 77: // right
        movex = 1;
        break;
    case 149: // right & up
        movex = 1;
        movey = -1;
        break;
    case 157: // right & down
        movex = 1;
        movey = 1;
        break;
    case 155: // left & up
        movex = -1;
        movey = 1;
        break;
    case 147: // left & down
        movex = -1;
        movey = -1;
        break;
    default:
        break;
    }
    if (movex != 0 || movey != 0)
    {
        step++; // move once step once
    }
    trackhead(head, movex, movey);
}
void trackhead(struct SNAKE *head, int movex, int movey)
{
    if (clickarrow != 0)
    {
        struct SNAKE *current = head->next;
        int prev_x = head->x;
        int prev_y = head->y;
        head->last_x = prev_x;
        head->last_y = prev_y;
        head->x += movex;
        head->y += movey;
        while (current != NULL) // track system, from the next of head to tail
        {
            int cur_x = current->x;
            int cur_y = current->y;
            current->x = prev_x;
            current->y = prev_y;
            current->last_x = cur_x;
            current->last_y = cur_y;
            prev_x = cur_x;
            prev_y = cur_y;
            current = current->next;
        }
    }
}
void ch_speed()
{
    int ori_speed = 50;
    speed = ori_speed;
    if ((GetAsyncKeyState(VK_UP) & 0x8000) && (GetAsyncKeyState(VK_RIGHT) & 0x8000) &&
        clickarrow == 149) // long press right & up
    {
        speed /= 5;
    }
    else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && (GetAsyncKeyState(VK_RIGHT) & 0x8000) &&
             clickarrow == 157) // long press right & down
    {
        speed /= 5;
    }
    else if ((GetAsyncKeyState(VK_UP) & 0x8000) && (GetAsyncKeyState(VK_LEFT) & 0x8000) &&
             clickarrow == 147) // long press left & up
    {
        speed /= 5;
    }
    else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && (GetAsyncKeyState(VK_LEFT) & 0x8000) &&
             clickarrow == 155) // long press left & up
    {
        speed /= 5;
    }
    else if ((GetAsyncKeyState(VK_UP) & 0x8000) && clickarrow == 72) // long press up
    {
        speed /= 2;
    }
    else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && clickarrow == 80) // long press down
    {
        speed /= 2;
    }
    else if ((GetAsyncKeyState(VK_LEFT) & 0x8000) && clickarrow == 75) // long press left
    {
        speed /= 10;
    }
    else if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) && clickarrow == 77) // long press right
    {
        speed /= 10;
    }
}
void fail(struct SNAKE *head, struct SNAKE *brick)
{
    if (head->x == 0 || head->x == maxwid || head->y == 0 || head->y == maxhet)
    {
        safe = 1; // bump into fence, fail
        return;
    }
    struct SNAKE *current = head->next;
    while (current != NULL) // from the next of head to tail
    {
        if (head->x == current->x && head->y == current->y)
        {
            safe = 1; // bump into snake body, fail
            return;
        }
        current = current->next;
    }
    current = brick;
    while (current != NULL) // from the next of head to tail
    {
        if (head->x == current->x && head->y == current->y)
        {
            safe = 1; // bump into birck, fail
            return;
        }
        current = current->next;
    }
    return; // game continue
}
void end(struct SNAKE **head, struct SNAKE **brick,
         struct SNAKE **swirl, struct SNAKE **wormhole)
{
    if (safe == 1)
    {
        COORD crddead;
        crddead.X = (*head)->x;
        crddead.Y = (*head)->y;
        colorprint(crddead, "?", 7, 0); // show dead where, fore: white, back: light yellow
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(handle, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(handle, &cursorInfo); // set cursor unvisible
        int arrow;
        COORD crdend;
        crdend.X = maxwid + 2;
        crdend.Y = 27;
        colorprint(crdend, "- GAME OVER -", FOREGROUND_GREEN, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
        // fore: green, back: light yellow
        crdend.Y = 29;
        colorprint(crdend, "LEAVE |", ori_foreground, ori_background);
        crdend.Y = 30;
        colorprint(crdend, "AGAIN |", ori_foreground, ori_background);
        crdend.Y = 29;
        while (_kbhit()) // clear kb input
        {
            getch();
        }
        while (1)
        {
            if (_kbhit())
            {
                arrow = getch();
                if (arrow == 224)
                {
                    arrow = getch();
                    if (arrow == 72 || arrow == 80) // up or down
                    {
                        switch (arrow) // do the arrow
                        {
                        case 72: // up
                            if (crdend.Y == 29)
                                crdend.Y = 29;
                            else
                                crdend.Y--;
                            break;
                        case 80: // down
                            if (crdend.Y == 30)
                                crdend.Y = 30;
                            else
                                crdend.Y++;
                            break;
                        }
                    }
                }
                else if (arrow == 13)
                {
                    switch (crdend.Y)
                    {
                    case 29: // leave
                        endcartoon();
                    case 30: // play again
                        clickarrow = 0;
                        point = 0, point_x = 0, point_y = 0;
                        step = 0;
                        safe = 0;
                        shuffle = 0;
                        freelist(head, maxwid / 2, maxhet / 2);
                        freelist(brick, 0, 0);
                        freelist(swirl, 0, 0);
                        freelist(wormhole, 0, 0);
                        system("cls");
                        setmode();
                        return;
                    }
                }
            }
            COORD crdblink;
            crdblink.X = crdend.X + 9;
            crdblink.Y = crdend.Y;
            colorprint(crdblink, "<<", 7, ori_background); // fore: white, back: gray
            Sleep(100);
            colorprint(crdblink, "  ", ori_foreground, ori_background); // fore: black, back: gray
            Sleep(100);
            COORD crd00 = {0, 0};
            SetConsoleCursorPosition(handle, crd00);
        }
    }
}
void endcartoon()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(handle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(handle, &cursorInfo); // set cursor unvisible
    int row = 0;
    int x = 0, y = 0;
    int right = csbi.srWindow.Right + 1;
    int bottom = csbi.srWindow.Bottom;
    for (row = 0; row < right / 2; row++)
    {
        for (y = 0; y <= bottom; y++)
        {
            COORD crdcartoon;
            crdcartoon.X = row;
            crdcartoon.Y = y;
            colorprint(crdcartoon, " ", 0, 0); // fore: black, back: black
        }
        for (y = 0; y <= bottom; y++)
        {
            COORD crdcartoon;
            crdcartoon.X = right - row;
            crdcartoon.Y = y;
            colorprint(crdcartoon, " ", 0, 0); // fore: black, back: black
        }
        Sleep(1);
    }
    exit(0);
}