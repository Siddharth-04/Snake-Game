#include <bits/stdc++.h>
#include "C:\raylib\raylib\src\raylib.h"
#include<raymath.h>
using namespace std;
Color green = {173,204,96,255};   //{red,green,blue,alpha};
Color darkGreen = {43,51,24,255}; //alpha --> transperancy

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool elementInDeque(Vector2 element,deque<Vector2> deque)
{

    for(unsigned int i=0 ; i < deque.size() ; i++){
        if(Vector2Equals(deque[i],element)){
            return true;
        }
    }

    return false;
}

bool eventTriggered(double interval)
{
    double currentTime = GetTime();

    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }

    return false;
}

class Snake{
    public:
    deque<Vector2> body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
    bool addSegment = false;
    Vector2 direction = {1,0};

    void Draw(){
        for(unsigned int i=0 ; i<body.size() ; i++)
        {
            float x = body[i].x;
            float y = body[i].y;

            Rectangle segment = Rectangle{offset+x*cellSize,offset + y*cellSize,(float)cellSize,(float)cellSize};

            //DrawRectangleRounded(Rectangle Rec,float roundness,int segments,Color color);
            DrawRectangleRounded(segment,0.7,7,darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0],direction));

        if(addSegment == true)
        {
            addSegment = false;
        }

        else
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        direction = {1,0};
    }
};

class Food{
    public:
    Vector2 position;
    Texture2D texture;
    
    Food(deque <Vector2> snakeBody)
    {
        Image image = LoadImage("pics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food(){
        UnloadTexture(texture);
    }

    void Draw(){
        //drawRectangle(int posx,int posy,int width,int height,Color color);
        //DrawRectangle(position.x*cellSize,position.y*cellSize,cellSize,cellSize,darkGreen);

        DrawTexture(texture,offset + position.x*cellSize,offset + position.y*cellSize,WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0,cellCount-1);
        float y = GetRandomValue(0,cellCount-1);

        return Vector2{x,y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();

        while(elementInDeque(position,snakeBody)){
            position = GenerateRandomCell();
        }

        return position;
    }
};

class Game{
    public:
    Snake snake  = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("sounds/eat.mp3");
        wallSound = LoadSound("sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if(running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
        
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();

        if(elementInDeque(snake.body[0],headlessBody)){
            GameOver();
        }
    }

    void CheckCollisionWithFood()
    {
        if(Vector2Equals(snake.body[0],food.position))
        {
            cout<<"Eating food"<<endl;
            score += 5;
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            PlaySound(eatSound);
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    void CheckCollisionWithEdges()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }

        if(snake.body[0].y == cellCount || snake.body[0].y == -1){
            GameOver();
        }
    }
};

  
int main()
{
    cout<<"Welcome to Snake Xenxia";
    InitWindow(2*offset + cellSize*cellCount,2*offset + cellSize*cellCount,"Snake Xenxia"); //size of the window of game
    SetTargetFPS(60);
    Game game = Game();

    while(WindowShouldClose() == false) //if user pressed esc or close the winow, then game stops
    {
    
        BeginDrawing();

        if(eventTriggered(0.2)){
            game.Update();
        }

        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0,-1};
            game.running = true;
        }

        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0,1};
            game.running = true;
        }

        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1,0};
            game.running = true;
        }

        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1,0};
            game.running = true;
        }
        //grid
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)cellSize*cellCount+10,(float)cellSize*cellCount+10},5,darkGreen);
        //DrawText(text,posX,posY,fontSize,darkGreen);
        DrawText("Snake Xenxia",offset-5,20,40,darkGreen);
        DrawText("Score",offset-5,offset + cellSize*cellCount + 10,40,darkGreen);
        DrawText(TextFormat("%i",game.score),offset+150,offset + cellSize*cellCount +10,40,darkGreen);

        game.Draw();
        

        EndDrawing();
    }

    CloseWindow();
    return 0;
}