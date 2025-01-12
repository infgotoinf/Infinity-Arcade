#include <iostream>
#include <windows.h> 
#include <ctime> 
#include <list>

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

enum Direction
{
    LEFT = -1, RIGHT = 1, UP = -2, DOWN = 2, NONE = 0
};

struct Snake
{
    std::list<COORD> snake;
    Direction direction;

    Snake() // Конструктор змейки
    {
        snake = { { 11, 16 }, { 11, 17 }, { 11, 18 } };
        direction = Direction::UP;
    }

    void drawSnake(int color = 41) // Отрисовка змейки (color = 0 для затирания)
    {
        for (COORD value : snake)
        {
            value.X *= 2;
            SetConsoleCursorPosition(console, value);
            // для вывода используем printf вместо std::cout
            printf("\033[%dm%c \033[0m", color, ' ');
        }
    }

    void grow() // Увеличение змейки
    {
        COORD nw = { snake.back().X, snake.back().Y};
        snake.push_back(nw);
    }


    void moveSnake() // Движение змейки
    {
        if (direction == LEFT) {
            snake.pop_back();
            COORD nw = { snake.front().X - 1, snake.front().Y };
            snake.push_front(nw);
        }
        else if (direction == RIGHT) {
            snake.pop_back();
            COORD nw = { snake.front().X + 1, snake.front().Y };
            snake.push_front(nw);
        }
        else if (direction == UP) {
            snake.pop_back();
            COORD nw = { snake.front().X, snake.front().Y - 1 };
            snake.push_front(nw);
        }
        else if (direction == DOWN) {
            snake.pop_back();
            COORD nw = { snake.front().X, snake.front().Y + 1 };
            snake.push_front(nw);
        }
    }

    bool checkApple(COORD apple) // Проверка на съедание яблока
    {
        return (snake.front().X == apple.X/2 && snake.front().Y == apple.Y ? true : false);
    }

    bool checkBounds(COORD size) // Проверка выхода за границу поля
    {
        if (snake.front().X == size.X || snake.front().X == 0 ||
            snake.front().Y == size.Y || snake.front().Y == 0) {
            return false;
        }
        else {
            return true;
        }
    }

    bool checkYourself() { // Проверка на то, что не ударился в себя
        int counter = -1;
        for (COORD i : snake) {
            if (i.X == snake.front().X && i.Y == snake.front().Y) {
                counter++;
            }
        }
        return counter;
    }
};

// Генерация нового яблока
COORD generateApple(COORD apple, COORD size, Snake s)
{
    // Проверка на то, что яблоко не в змейке
    /*auto lambda { [](COORD appl, std::list<COORD> snak)
        {
            for (COORD i : snak) {
                if ((i.X == appl.X/2 - 1 || i.X == appl.X/2 || i.X == appl.X/2 + 1) &&
                    (i.Y == appl.Y - 1 || i.X == appl.Y || i.X == appl.Y + 1)){
                    return true;
                }
                return false;
            }
        }
    };
    do {
        short X = (rand() % (size.X - 2) + 1) * 2;
        short Y = rand() % (size.Y - 2) + 1;
        apple = { X, Y };
    } while (lambda(apple, s.snake));*/

    // Проверка на то, что яблоко не в змейке V2
    bool stoper = true;
    do { 
        short X = rand() % (size.X - 2) + 1;
        short Y = rand() % (size.Y - 2) + 1;
        apple = { X, Y };
        for (COORD i : s.snake) {
            if ((i.X == apple.X - 1 || i.X == apple.X || i.X == apple.X + 1) &&
                (i.Y == apple.Y - 1 || i.Y == apple.Y || i.Y == apple.Y + 1)) {
            }
            stoper = false;
            break;
        }
    } while (stoper);

    apple.X *= 2;
    return apple;
}

// Отрисовка яблока
void drawApple(COORD apple)
{
    SetConsoleCursorPosition(console, apple);
    printf("\033[%dm%c \033[0m", 102, ' ');
}

// Отрисовка рамки
auto drawFrame(COORD size, int design)
{
    for (int i = 0; i <= size.X; i++) {
        printf("\033[100m  \033[0m");
    }
    std::cout << '\n';
    switch (design) {
    case 0: // Обычное поле
        for (int i = 1; i < size.Y; i++) {
            for (int i2 = 0; i2 <= size.X; i2++) {
                printf(i2 == 0 || i2 == size.X ? "\033[100m  \033[0m" : "  ");
            }
            std::cout << '\n';
        }
        for (int i = 0; i <= size.X; i++) {
            printf("\033[100m  \033[0m");
        }
        break;
    case 1: // Шахматное
        int** matrix = new int*[size.Y];
        for (int i = 1; i < size.Y; i++) {
            matrix[i] = new int[size.Y];
            for (int i2 = 0; i2 <= size.X; i2++) {
                if (i2 == 0 || i2 == size.X) {
                    printf("\033[100m  \033[0m");
                }
                else {
                    int i_1 = i + 2;
                    int i_2 = i2 + 2;
                    if ((i_1 % 6 > 2 && i_2 % 6 > 2) ||
                        (i_1 % 6 < 3 && i_2 % 6 < 3))
                    {
                        matrix[i][i2] = 0;
                        printf("\033[107m  \033[0m");
                    }
                    else {
                        matrix[i][i2] = 1;
                        printf("\033[47m  \033[0m");
                    }
                }
            }
            std::cout << '\n';
        }
        for (int i = 0; i <= size.X; i++) {
            printf("\033[100m  \033[0m");
        }
        return matrix;
    }
}

void draw_behind_tail(int** matrix, COORD tail) { // Зарисовывание поля за змейкой
    tail.X *= 2;
    SetConsoleCursorPosition(console, tail);
    tail.X /= 2;
    switch (matrix[tail.Y][tail.X]) {
    case 0:
        printf("\033[107m  \033[0m");
        break;
    case 1:
        printf("\033[47m  \033[0m");
    }
}

bool stop = false;

void getKey(Snake* snake)
{
    Direction newDir = Direction::NONE;

    if (GetAsyncKeyState(VK_UP)) // Для управления используются стрелочки
        newDir = Direction::UP;
    else if (GetAsyncKeyState(VK_LEFT))
        newDir = Direction::LEFT;
    else if (GetAsyncKeyState(VK_DOWN))
        newDir = Direction::DOWN;
    else if (GetAsyncKeyState(VK_RIGHT))
        newDir = Direction::RIGHT;

    if (GetAsyncKeyState(VK_ESCAPE) || GetAsyncKeyState(VK_CANCEL))
    {
        stop = true;
        return;
    }

    // Убедимся, что было что-то нажато
    if (newDir == Direction::NONE) return;

    // Убедимся что направление не противоположное
    if (snake->direction != -newDir) snake->direction = newDir;
}

short center(std::string words, int coord) {
    return coord - words.length() / 2 + 1;
}

int main()
{
    double bestScore = 0;
select:
    for (int i = 0; i < 2; i++) { // Цикл фор чтобы бага при запуске экзешника не было
    SetConsoleCursorPosition(console, { 36, 2 }); std::cout << "\033[41m      \033[40m" << "            " <<                                                              "\033[41m      \033[40m" << "            " <<                                        "\033[100m  \033[40m" << "    " <<   "\033[41m      \033[40m";
    SetConsoleCursorPosition(console, { 36, 3 }); std::cout << "\033[41m  \033[40m" << "            " <<                                             "\033[100m  \033[40m  \033[41m  \033[40m" << "                " <<                                        "\033[100m  \033[40m" << "    " <<   "\033[41m  \033[40m    \033[100m    \033[40m";
    SetConsoleCursorPosition(console, { 36, 4 }); std::cout << "\033[41m      \033[40m" << "  " << "\033[42m    " <<                               "\033[100m      " <<   "\033[41m      \033[100m    \033[40m" << "    " <<                 "\033[42m    " << "\033[100m  \033[40m  \033[100m  " << "\033[41m      " <<   "\033[100m  \033[40m  \033[100m  \033[40m";
    SetConsoleCursorPosition(console, { 36, 5 }); std::cout << "\033[41m  \033[40m" << "    " << "\033[42m  \033[40m  \033[42m  \033[40m" << "  " << "\033[100m  \033[40m      \033[41m  \033[100m  \033[40m  \033[100m  " <<    "\033[42m  \033[40m  \033[42m  \033[100m    \033[40m" << "  " <<   "\033[41m  \033[40m    \033[100m  \033[40m";
    SetConsoleCursorPosition(console, { 36, 6 }); std::cout << "\033[41m      \033[40m" << "  " << "\033[42m    \033[40m" << "  " <<                 "\033[100m    " <<   "\033[41m      \033[100m  \033[40m  \033[100m  \033[40m" << "  " << "\033[42m    " << "\033[100m  \033[40m  \033[100m  " << "\033[41m      " <<   "\033[100m  \033[40m" << "\tV1.0";
    // Выбор сложности и дизайна
    SetConsoleCursorPosition(console, { 0, 0 });
    std::cout << "(USE ARROWS ON YOUR KEYBOARD)\n\nPLEASE SELECT YOUR DIFFICULTY\n\n";
    std::cout << "1: CASUAL\n2: COOL GUY\n3: SPEEDRUNNER\n\n";
    std::cout << "(PRESS [ENTER] TO CONTINUE)";
    SetConsoleCursorPosition(console, { 19, 3 });
    std::cout << "AND DESIGN:";
    SetConsoleCursorPosition(console, { 17, 5 });
    std::cout << "1: CLASSICAL";
    SetConsoleCursorPosition(console, { 17, 6 });
    std::cout << "2: CHECKMATE";
    if (i == 0) {
        system("cls");
    }
    }
    int difficulty = 0;
    int design = 0;
    bool switcher = true;
    while (true) {
        if (GetAsyncKeyState(VK_UP)) {
            if (switcher) {
                if (difficulty != 0) {
                    difficulty--;
                }
            }
            else {
                if (design != 0) {
                    design--;
                }
            }
        }
        else if (GetAsyncKeyState(VK_DOWN)) {
            if (switcher) {
                if (difficulty != 2) {
                    difficulty++;
                }
            }
            else {
                if (design != 1) {
                    design++;
                }
            }
        }
        else if (GetAsyncKeyState(VK_DOWN)) {
            if (switcher) {
                if (difficulty != 2) {
                    difficulty++;
                }
            }
        }
        else if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_RIGHT)) {
            switcher = !switcher;
        }
        else if (GetAsyncKeyState(VK_RETURN)) {
            break;
        }
        switch (difficulty) {
        case 0:
            SetConsoleCursorPosition(console, { 0, 4 });
            printf("\033[30m\033[107m1: CASUAL\033[0m\033[40m\n");
            printf("2: COOL GUY");
            break;
        case 1:
            SetConsoleCursorPosition(console, { 0, 4 });
            printf("1: CASUAL\n");
            printf("\033[30m\033[107m2: COOL GUY\033[0m\033[40m\n");
            printf("3: SPEEDRUNNER");
            break;
        case 2:
            SetConsoleCursorPosition(console, { 0, 5 });
            printf("2: COOL GUY\n");
            printf("\033[30m\033[107m3: SPEEDRUNNER\033[0m\033[40m");
            break;
        }

        switch (design) {
        case 0:
            SetConsoleCursorPosition(console, { 17, 5 });
            printf("\033[30m\033[107m1: CLASSICAL\033[0m\033[40m");
            SetConsoleCursorPosition(console, { 17, 6 });
            printf("2: CHECKMATE");
            break;
        case 1:
            SetConsoleCursorPosition(console, { 17, 5 });
            printf("1: CLASSICAL");
            SetConsoleCursorPosition(console, { 17, 6 });
            printf("\033[30m\033[107m2: CHECKMATE\033[0m\033[40m");
            break;
        }

        switch (switcher) {
        case true:
            SetConsoleCursorPosition(console, { 19, 2 });
            printf("\033[30m\033[107mDIFFICULTY\033[0m\033[40m");
            SetConsoleCursorPosition(console, { 23, 3 });
            printf("DESIGN");
            break;
        case false:
            SetConsoleCursorPosition(console, { 19, 2 });
            printf("DIFFICULTY");
            SetConsoleCursorPosition(console, { 23, 3 });
            printf("\033[30m\033[107mDESIGN\033[0m\033[40m");
            break;
        }
        Sleep(100);
    }
    double step = (difficulty == 0 ? 0.5 : (difficulty == 1 ? 1 : 1.5));
    COORD size = { 22, 22 }; // Размер окна, включая границы
    int** matrix = drawFrame(size, design);
    system("cls");


    game:
    drawFrame(size, design); // Граница
    std::cout << "\nDifficulty: ";
    std::cout << (difficulty == 0 ? "CASUAL" : (difficulty == 1 ? "COOL GUY" : "SPEEDRUNNER"));
    std::cout << "\nScore Multiplier: " << step << 'X';

    double score = 0;
    COORD score_pos = { 25, size.Y + 1 };
    SetConsoleCursorPosition(console, score_pos);
    std::cout << "Score: " << score;
    score_pos.Y += 1;
    SetConsoleCursorPosition(console, score_pos);
    std::cout << "Best Score: " << bestScore;
    score_pos.Y -= 1;
    score_pos.X += 7;

    srand(time(0)); // Функция для инициализации генератора случайных чисел

    Snake s; // Создаем змейку

    COORD apple = {0, 0};
    apple = generateApple(apple, size, s); // Отрисовка яблочка

    stop = false;

    int time_to_sleep = (difficulty == 0 ? 200 : (difficulty == 1 ? 100 : 50));

    s.drawSnake(41);
    drawApple(apple);
    short thisX = center("PRESS AN ARROW TO START", size.X);
    SetConsoleCursorPosition(console, { thisX, 11});
    printf("PRESS AN ARROW TO START");
    Sleep(250);

    while (!(GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_RIGHT)));
    SetConsoleCursorPosition(console, { 0, 0 });
    drawFrame(size, design);

    while (!stop) // Пока игра не остановлена
    {
        getKey(&s);
        s.drawSnake(0); // Удаляем змейку
        
        if (s.checkApple(apple))
        {
            s.grow();
            score += step;
            SetConsoleCursorPosition(console, score_pos);
            printf("%d.%d", int(score), int(score*10)-int(score)*10);
            apple = generateApple(apple, size, s);
        }
        drawApple(apple);

        COORD old_tail = s.snake.back();

        s.moveSnake();
        
        stop = !s.checkBounds(size) || s.checkYourself();
        
        s.drawSnake(41); // Рисуем змейку красного цвета

        if (design == 1) {
            draw_behind_tail(matrix, old_tail);
        }
        
        Sleep(time_to_sleep);
    }
    if (bestScore < score) {
        bestScore = score;
        score_pos.X -= 7;
        score_pos.Y += 1;
        SetConsoleCursorPosition(console, score_pos);
        std::cout << "Best score: " << bestScore << " *New!*";
    }

    SetConsoleCursorPosition(console, { center("GAME OVER!", size.X), 10});
    printf("GAME OVER!\n");
    int num = rand() % 30;
    std::string commentary;
    switch (num) {
    case 0: commentary = "Skill issue!"; break;
    case 1: commentary = "XDXDXDXDDXDXD"; break;
    case 2: commentary = "Cause fuck you!"; break;
    case 3: commentary = { char(0), char(0), char(0), char(0), char(0) }; break;
    case 4: commentary = "Use Alt+F4 to rage quit!"; break;
    case 5: commentary = "Cry about it!"; break;
    case 6: commentary = "Eat your legs!"; break;
    case 7: commentary = "*Sigma snake was rizzed by Skibidy Toilets*"; break;
    case 8: commentary = "A snail walks into the bar!"; break;
    case 9: commentary = "Dead because of dihotomy!"; break;
    case 10: commentary = "Disagree? Make a game yourself!"; break;
    case 11: commentary = "What was close!"; break;
    case 12: commentary = "\"That was my try number 777!\" - you"; break;
    case 13: commentary = "Buy me some bread, I'm hungry!"; break;
    case 14: commentary = "BOOOOOM!!!"; break;
    case 15: commentary = "UwU or QwQ?"; break;
    case 16: commentary = "FrEaDy FaZbEaR?!?!?"; break;
    case 17: commentary = "Just use firearms!"; break;
    case 18: commentary = "Madness is an exact repetition..."; break;
    case 19: commentary = "Unluck!"; break;
    case 20: commentary = "Go play roblox!"; break;
    case 21: commentary = "Why don't you just brake the wall?"; break;
    case 22: commentary = "Pay to win bullshit!"; break;
    case 23: commentary = ":P"; break;
    case 24: commentary = "+Social credit and 20 catgirl-wifes!"; break;
    case 25: commentary = "Pray to SnakeGods!"; break;
    case 26: commentary = "Just don't eat yourself and the walls!"; break;
    case 27: commentary = "Go call me -> 8800553535!"; break;
    case 28: commentary = "Did you tried to go forward?"; break;
    case 29: commentary = "Don't worry, he will reborn!"; break;
    }
    SetConsoleCursorPosition(console, { center(commentary, size.X), 11});
    std::cout << commentary;
    SetConsoleCursorPosition(console, { center("[BACKSPACE] - menu; [ENTER] - continue", size.X), 12 });
    printf("[BACKSPACE] - menu; [ENTER] - continue");
    while (true) {
        if (GetAsyncKeyState(VK_BACK)) {
            system("cls");
            goto select;
        }
        else if (GetAsyncKeyState(VK_RETURN)) {
            system("cls");
            goto game;
        }
    }
    return 0;
}