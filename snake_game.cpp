#include <iostream>
#include <cstdlib>
#include <ctime>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

#define H 22
#define W 22

using namespace std;

class chessboard {
public:
    char qp[H][W];
    int i, j, x1, y1;
    chessboard();
    void food();
    void prt(int grade, int score, int gamespeed);
};

chessboard::chessboard() {
    for(i = 1; i <= H - 2; i++) 
        for(j = 1; j <= W - 2; j++) 
            qp[i][j] = ' ';
    
    for(i = 0; i <= H - 1; i++) 
        qp[0][i] = qp[H - 1][i] = '#';
    
    for(i = 1; i <= H - 2; i++) 
        qp[i][0] = qp[i][W - 1] = '#';
    
    food();
}

void chessboard::food() {
    srand(time(0));
    do {
        x1 = rand() % (W - 2) + 1;
        y1 = rand() % (H - 2) + 1;
    } while(qp[x1][y1] != ' ');
    qp[x1][y1] = '$';
}

void chessboard::prt(int grade, int score, int gamespeed) {
    system("clear");  // Linux 环境下清屏
    cout << endl;
    for(i = 0; i < H; i++) {
        cout << "\t";
        for(j = 0; j < W; j++) 
            cout << qp[i][j] << ' ';
        
        if(i == 0) cout << "\tGrade: " << grade;
        if(i == 2) cout << "\tScore: " << score;
        if(i == 4) cout << "\tAutomatic forward";
        if(i == 5) cout << "\ttime interval: " << gamespeed << "ms";
        cout << endl;
    }
}

class snake: public chessboard {
public:
    int zb[2][100];
    long start;
    int head, tail, grade, score, gamespeed, length, timeover, x, y;
    char direction;
    snake();
    void move();
    bool kbhit();  // 添加 kbhit 函数声明
};

// 获取单个键盘输入
char getInput() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);  // 获取当前终端设置
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // 禁用行模式和回显
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // 设置新的终端设置
    ch = getchar();  // 获取单个字符
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // 恢复原始设置
    return ch;
}

snake::snake() {
    cout << "\n\n\t\tThe game is about to begin!" << endl;
    for(i = 3; i >= 0; i--) {
        start = clock();
        while(clock() - start <= 1000);  // 1 second countdown
        system("clear");  // Linux 环境下清屏
        if(i > 0)
            cout << "\n\n\t\tCountdown: " << i << endl;
    }
    for(i = 1; i <= 3; i++)
        qp[1][i] = '*';
    qp[1][4] = '@';
    for(i = 0; i < 4; i++) {
        zb[0][i] = 1;
        zb[1][i] = i + 1;
    }
}

void snake::move() {
    score = 0;
    head = 3, tail = 0;
    grade = 1, length = 4;
    gamespeed = 500;
    direction = 'd';  // 默认向右
    while(1) {
        timeover = 1;
        start = clock();
        while((timeover = (clock() - start <= gamespeed)) && !kbhit());  // 等待玩家输入
        if(timeover) {
            direction = getInput();  // 获取用户输入
        }
        
        switch(direction) {
        case 'w': 
            x = zb[0][head] - 1; 
            y = zb[1][head]; 
            break;
        case 's': 
            x = zb[0][head] + 1; 
            y = zb[1][head]; 
            break;
        case 'a': 
            x = zb[0][head]; 
            y = zb[1][head] - 1; 
            break;
        case 'd': 
            x = zb[0][head]; 
            y = zb[1][head] + 1; 
            break;
        }

        if(x == 0 || x == 21 || y == 0 || y == 21) {
            cout << "\tGame over!" << endl;
            break;
        }
        
        if(qp[x][y] != ' ' && !(x == x1 && y == y1)) {
            cout << "\tGame over!" << endl;
            break;
        }
        
        if(x == x1 && y == y1) {
            length++;
            score += 100;
            if(length >= 8) {
                length -= 8;
                grade++;
                if(gamespeed >= 200)
                    gamespeed = 550 - grade * 50;
            }
            qp[x][y] = '@';
            qp[zb[0][head]][zb[1][head]] = '*';
            head = (head + 1) % 100;
            zb[0][head] = x;
            zb[1][head] = y;
            food();
            prt(grade, score, gamespeed);
        }
        else {
            qp[zb[0][tail]][zb[1][tail]] = ' ';
            tail = (tail + 1) % 100;
            qp[zb[0][head]][zb[1][head]] = '*';
            head = (head + 1) % 100;
            zb[0][head] = x;
            zb[1][head] = y;
            qp[zb[0][head]][zb[1][head]] = '@';
            prt(grade, score, gamespeed);
        }
    }
}

// 使用 termios 实现 kbhit 的功能
bool snake::kbhit() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);  // 禁用行模式和回显
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    if(ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
}

int main() {
    chessboard cb;
    snake s;
    s.move();
}

