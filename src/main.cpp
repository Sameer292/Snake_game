#include <raylib.h>
#include <iostream>
#include <deque>
#include <raymath.h>

using namespace std;
enum GameScreen
{
  TITLE = 0,
  MENU,
  GAMEPLAY,
  ENDING
} currentscreen;
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};
int cellsize = 20;
int cellcount = 30;
int offset = 50;
double lastupdatetime = 0;
double lastupdatetime1 = 0;
double lastupdatetime2 = 0;
double lastupdatetime3 = 0;
bool triggered=false;
int FinalScore = 0;

bool eventtriggered(double interval)
{
  double currenttime = GetTime();
  if (currenttime - lastupdatetime >= interval)
  {
    lastupdatetime = currenttime;
    return true;
  }
  else
    return false;
}
bool eventtriggered1(double interval)
{
  double currenttime1 = GetTime();
  if (currenttime1 - lastupdatetime1 >= interval)
  {
    lastupdatetime1 = currenttime1;
    return true;
  }
  else
    return false;
}
double now = 0;

bool eventtriggered2(double interval)
{
  if ((now =now+ (1 / 60)) < interval)
  {
  return true;
  }
  else
  {
    now=0;
    return false;
  }

  // double currenttime2 = GetTime();
  // if (currenttime2 - lastupdatetime2 >= interval)
  // {
  //   lastupdatetime2 = currenttime2;
  //   return true;
  // }
  // else
  //   return false;
}

// bool eventtriggered3(double interval)
// {
//   double currenttime3 = GetTime();
//   if (currenttime3 - lastupdatetime3 >= interval)
//   {
//     lastupdatetime3 = currenttime3;
//     return true;
//   }
//   else
//     return false;
// }

// bool eventtriggeredspecial(double duration)
// {
// double now;
//   if (   (now+(1/60)) <= duration )
//   {
//     return true;
//   }
//   else
//   {
//     now=0;
//     return false;
//   }

// }

bool dequeElem(Vector2 elem, deque<Vector2> deq)
{
  for (unsigned int i = 0; i < deq.size(); i++)
  {
    if (Vector2Equals(deq[i], elem))
    {
      return true;
    }
  }
  return false;
}

class Snake
{
public:
  deque<Vector2> body = {Vector2{3, 4}, Vector2{2, 4}, Vector2{1, 4}};
  Vector2 direction = {1, 0};
  bool addSegment = false;

  void Draw()
  {
    for (unsigned int i = 0; i < body.size(); i++)
    {
      float x = body[i].x;
      float y = body[i].y;
      Rectangle segment = Rectangle{offset + x * cellsize, offset + y * cellsize, (float)cellsize, (float)cellsize};
      DrawRectangleRounded(segment, 0.5, 6, BLUE);
    }
  }
  void update()
  {
    body.push_front(Vector2Add(body[0], direction));
    if (addSegment == true)
    {

      addSegment = false;
    }
    else
    {
      body.pop_back();
      // body.push_front(Vector2Add(body[0], direction));
    }
  }
  void reset()
  {
    body = {Vector2{3, 4}, Vector2{2, 4}, Vector2{1, 4}};
    direction = {1, 0};
  }
};

class Food
{
public:
  Vector2 position;
  Vector2 spcposition;

  Texture2D texture1;
  Texture2D texture2;
  Vector2 generaterandomcell()
  {

    float x = GetRandomValue(0, 50 - 1);
    float y = GetRandomValue(0, cellcount - 1);
    return Vector2{x, y};
  }
  Vector2 generateRandomPos(deque<Vector2> snakebody)
  {

    Vector2 position = generaterandomcell();
    while (dequeElem(position, snakebody))
    {
      position = generaterandomcell();
    }
    return position;
  }
  Food(deque<Vector2> snakebody)
  {
    Image image1 = LoadImage("resources/food.png");
    Image image2 = LoadImage("resources/specialfood.png");
    texture1 = LoadTextureFromImage(image1);
    texture2 = LoadTextureFromImage(image2);
    UnloadImage(image1);
    UnloadImage(image2);
    position = generateRandomPos(snakebody);
  }
  ~Food()
  {
    UnloadTexture(texture1);
    UnloadTexture(texture2);
  }
  void Draw(deque<Vector2> snakebody)
  {
    DrawTexture(texture1, offset + position.x * cellsize, offset + position.y * cellsize, WHITE);

    if (eventtriggered1(5))
    {
      specialfood(snakebody);
      triggered=true;
    }
  }
  void specialfood(deque<Vector2> snakebod)
  {
    cout << "ENTERR!!!" << endl;
    spcposition = generateRandomPos(snakebod);
  }
  void drawspc()
  {
    DrawTexture(texture2, offset + spcposition.x * cellsize, offset + spcposition.y * cellsize, WHITE);
  }
};

class Game
{
public:
  Snake snake = Snake();
  Food food = Food(snake.body);
  bool running = false;
  int score = 0;
  Sound eat;
  Sound die;
  Game()
  {
    InitAudioDevice();
    eat = LoadSound("resources/eat.mp3");
    die = LoadSound("resources/die.mp3");
  }
  ~Game()
  {
    UnloadSound(eat);
    UnloadSound(die);
    CloseAudioDevice();
  }
  void draw()
  {
    food.Draw(snake.body);

    snake.Draw();
    if (triggered && eventtriggered2(3))
    {
      
      // cout << "YEEE!!!" << endl;
      food.drawspc();
      if(!eventtriggered(3))
      triggered = false;
    }
  }
  void update()
  {
    if (running)
    {
      snake.update();
      checkcollisionswithfood();
      checkcollisionswithedges();
      checkcollisionswithbody();
    }
  }
  void checkcollisionswithfood()
  {
    if (Vector2Equals(snake.body[0], food.position))
    {
      food.position = food.generateRandomPos(snake.body);
      snake.addSegment = true;
      score++;
      PlaySound(eat);
    }
  }
  void checkcollisionswithedges()
  {
    if (snake.body[0].x == 50 || snake.body[0].x == -1)
    {
      gameover();
    }
    if (snake.body[0].y == cellcount || snake.body[0].y == -1)
    {
      gameover();
    }
  }
  void checkcollisionswithbody()
  {
    deque<Vector2> headbody = snake.body;
    headbody.pop_front();
    if (dequeElem(snake.body[0], headbody))
    {
      gameover();
    }
  }
  void gameover()
  {
    snake.reset();
    food.position = food.generateRandomPos(snake.body);
    running = false;
    PlaySound(die);
    FinalScore = score;
    score = 0;

    currentscreen = ENDING;
  }
};

int main()
{
  InitWindow(2 * offset + 1000, 2 * offset + 600, "Snake");
  SetTargetFPS(60);
  currentscreen = TITLE;
  Game game = Game();
  // game loop
  while (WindowShouldClose() == false)
  {

    switch (currentscreen)
    {
    case (TITLE):
    {
      if (IsKeyPressed(KEY_ENTER))
      {
        game.running = false;
        currentscreen = MENU;
      }
    }
    break;
    case (MENU):
    {
      if (IsKeyPressed(KEY_SPACE))
      {
        game.running = false;
        currentscreen = GAMEPLAY;
      }
    }

    default:
      break;
    }

    BeginDrawing();
    switch (currentscreen)
    {
    case (TITLE):
    {
      DrawRectangle(0, 0, 2 * offset + 1000, 2 * offset + 600, WHITE);
      DrawText("SNAKE", 482, 230, 40, BLACK);
      DrawText("PRESS ENTER to play", 437, 300, 20, DARKGREEN);
    }
    break;
    case (MENU):
    {
      DrawRectangle(0, 0, 2 * offset + 1000, 2 * offset + 600, WHITE);
      DrawText("Modes", 482, 230, 25, BLACK);
      DrawText("Options", 482, 255, 25, BLACK);
    }
    break;
    case (GAMEPLAY):
    {

      if (eventtriggered(0.1))
      {
        game.update();
      }

      if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
      {
        game.snake.direction = {0, -1};
        game.running = true;
      }
      else if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
      {

        game.snake.direction = {0, 1};
        game.running = true;
      }
      else if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
      {

        game.snake.direction = {-1, 0};
        game.running = true;
      }
      else if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
      {

        game.snake.direction = {1, 0};
        game.running = true;
      }
      if (IsKeyPressed(KEY_SPACE))
      {
        game.running = !game.running;
      }

      ClearBackground(green);

      game.draw();
      DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)1010, (float)cellsize * cellcount + 10}, 5, darkGreen);
      DrawText("Snake", 45, 10, 40, darkGreen);
      DrawText(TextFormat("Score: %i", game.score), 500, 655, 35, darkGreen);
      DrawText("Press SPACE to pause/resume.", 725, 10, 20, BLACK);
    }
    break;
    case (ENDING):
    {
      DrawRectangle(0, 0, 2 * offset + 1000, 2 * offset + 600, WHITE);
      // DrawRectangle(0, 0, 1000, 600, GREEN);
      DrawText("GAME OVER!!!", 416, 230, 40, RED);
      DrawText(TextFormat("Score: %i", FinalScore), 412, 280, 25, BLACK);
      DrawText("PRESS SPACE to play again", 407, 320, 20, DARKGREEN);
      if (IsKeyPressed(KEY_SPACE))
        currentscreen = GAMEPLAY;
    }
    default:
      break;
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}