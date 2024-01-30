#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h> 
using namespace std;
#include <vector>

const int screen_width = 700;
const int screen_height = 500;
const int block_size = 20;
const int border_size = 10;

struct SnakeSegment {
    int x, y;
};

// Obstacles
SDL_Rect obstacle1{100, 120, 200, 20};  // x, y, width, height
SDL_Rect obstacle2{420, 360, 200, 20};
SDL_Rect obstacle3{300, 240, 100, 20};

// Initialization
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;
SDL_Texture* scoreTexture;
SDL_Rect scoreRect = {30, 30, 0, 0};
SDL_Texture* backgroundTexture;
SDL_Texture* snakeTexture;
SDL_Texture* foodTexture;
int score = 0;

// Function prototypes
void render(SDL_Renderer* renderer, const vector<SnakeSegment>& snake, const SDL_Point& food, const SDL_Point& bonusFood);
void update(vector<SnakeSegment>& snake, SDL_Point& food, SDL_Point& bonusFood, SDL_Keycode direction, int& score, bool& running, bool& bonusFoodActive);
bool checkCollision(const vector<SnakeSegment>& snake, int x, int y);
void spawnBonusFood(SDL_Point& bonusFood, const vector<SnakeSegment>& snake, const SDL_Point& food);
void displayGameOver();

//Main Function
int main(int argc, char* argv[]) 
{
    // Initialize SDL, SDL_ttf, and SDL_mixer
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0 || TTF_Init() != 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        return 1;
    }

    // Create window and renderer
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load font
    font = TTF_OpenFont("Gellisto.ttf", 24); 
    if (!font){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    // Load background image
    SDL_Surface* backgroundSurface = SDL_LoadBMP("background.bmp"); 
    if (!backgroundSurface) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

 /*   // Load snake image
    SDL_Surface* snakeSurface = SDL_LoadBMP("images (1).bmp"); 
    if (!snakeSurface) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
    snakeTexture = SDL_CreateTextureFromSurface(renderer, snakeSurface);
    SDL_FreeSurface(snakeSurface);

    // Load food image
    SDL_Surface* foodSurface = SDL_LoadBMP("images (1).bmp"); 
    if (!foodSurface) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
    foodTexture = SDL_CreateTextureFromSurface(renderer, foodSurface);
    SDL_FreeSurface(foodSurface);
*/    
    vector<SnakeSegment> snake{{15, 15}};
    SDL_Point food = {15, 10};
    SDL_Keycode direction = SDLK_RIGHT;
    bool running = true;
    bool bonusFoodActive = false;
    SDL_Point bonusFood = {-1, -1};

    // Main game loop
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (direction != SDLK_DOWN)
                            direction = SDLK_UP;
                        break;
                    case SDLK_DOWN:
                        if (direction != SDLK_UP)
                            direction = SDLK_DOWN;
                        break;
                    case SDLK_LEFT:
                        if (direction != SDLK_RIGHT)
                            direction = SDLK_LEFT;
                        break;
                    case SDLK_RIGHT:
                        if (direction != SDLK_LEFT)
                            direction = SDLK_RIGHT;
                        break;
                    default:
                        break;
                }
            }
        }

        // Update game logic
        update(snake, food, bonusFood, direction, score, running, bonusFoodActive);

        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render background
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        // Render objects
        render(renderer, snake, food, bonusFood);

        // Render score 
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

        // Present
        SDL_RenderPresent(renderer);

        // Speed of the game
        SDL_Delay(200);
    }

    // Clean up resources
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(snakeTexture);
    SDL_DestroyTexture(foodTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void render(SDL_Renderer* renderer, const vector<SnakeSegment>& snake, const SDL_Point& food, const SDL_Point& bonusFood) 
{
    // Render snake
    for (const auto& segment : snake) {
        SDL_Rect snakeRect = {segment.x * block_size, segment.y * block_size, block_size, block_size};
        SDL_RenderCopy(renderer, snakeTexture, NULL, &snakeRect);
    }

    // Render food
    // Load food image
    SDL_Surface* foodSurface = SDL_LoadBMP("images (1).bmp"); 
    
    foodTexture = SDL_CreateTextureFromSurface(renderer, foodSurface);
    SDL_FreeSurface(foodSurface);
    SDL_Rect foodRect = {food.x * block_size, food.y * block_size, block_size, block_size};
    SDL_RenderCopy(renderer, foodTexture, NULL, &foodRect);


    // Render bonus food
    SDL_Rect bonusFoodRect = {bonusFood.x * block_size, bonusFood.y * block_size, block_size, block_size};
    SDL_RenderCopy(renderer, foodTexture, NULL, &bonusFoodRect);

    // Render obstacles
    SDL_SetRenderDrawColor(renderer, 0, 51, 0, 255);
    SDL_RenderFillRect(renderer, &obstacle1);
    SDL_RenderFillRect(renderer, &obstacle2);
    SDL_RenderFillRect(renderer, &obstacle3);
}

void update(vector<SnakeSegment>& snake, SDL_Point& food, SDL_Point& bonusFood, SDL_Keycode direction,
            int& score, bool& running, bool& bonusFoodActive) {
    // Update snake's position based on the direction
    int headX = snake.front().x;
    int headY = snake.front().y;

    switch (direction) {
        case SDLK_UP:
            headY--;
            break;
        case SDLK_DOWN:
            headY++;
            break;
        case SDLK_LEFT:
            headX--;
            break;
        case SDLK_RIGHT:
            headX++;
            break;
        default:
            break;
    }

    // Check collisions with food 
    if (headX == food.x && headY == food.y) {
        // Snake ate the food, so increase its length and generate new food
        SnakeSegment newSegment = {headX, headY};
        snake.insert(snake.begin(), newSegment);
        food.x = rand() % (screen_width / block_size);
        food.y = rand() % (screen_height / block_size);
        score++;

        // Check for bonus food activation
        if (score % 2 == 0 && !bonusFoodActive) {
            bonusFood.x = rand() % (screen_width / block_size);
            bonusFood.y =  rand() % (screen_height / block_size);
            bonusFoodActive = true;
        }
    } else {
        // Wrap around the screen if the head hits the boundary
        if (headX < 0) {
            headX = screen_width/ block_size;
        } else if (headX >= screen_width/block_size) {
            headX = 0;
        }
        if (headY < 0) {
            headY = screen_height/block_size ;
        } else if (headY >= screen_height/block_size ) {
            headY = 0;
        }

        // Check for collision with the snake's body and obstacles
        if (checkCollision(snake, headX, headY)) {
            running = false;
            return;
        }

        // Movement 
        SnakeSegment newHead = {headX, headY};
        snake.insert(snake.begin(), newHead);
        snake.pop_back();
    }

    // Check for collision with bonus food
    if (bonusFoodActive && headX == bonusFood.x && headY == bonusFood.y) {
        score += 10;

        // Snake ate the bonus food, so increase its length and deactivate bonus food
        SnakeSegment newSegment = {headX, headY};
        snake.insert(snake.begin(), newSegment);
        bonusFoodActive = false;
        bonusFood = {-1, -1};
    }

    // Render the score
    string scoreText = "Score: " + to_string(score);
    SDL_Color textColor = {51, 51, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    scoreTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

bool checkCollision(const std::vector<SnakeSegment>& snake, int x, int y) 
{
    // Check for collision with the snake's body
    for (size_t i = 1; i < snake.size(); ++i) {
        if (snake[i].x == x && snake[i].y == y) {
            return true;
        }
    }

    // Check for collision with obstacles
    if ((x >= obstacle1.x / block_size && x < (obstacle1.x + obstacle1.w) / block_size) &&
        (y >= obstacle1.y / block_size && y < (obstacle1.y + obstacle1.h) / block_size)) {
        return true;
    }

    if ((x >= obstacle2.x / block_size && x < (obstacle2.x + obstacle2.w) / block_size) &&
        (y >= obstacle2.y / block_size && y < (obstacle2.y + obstacle2.h) / block_size)) {
        return true;
    }

    if ((x >= obstacle3.x / block_size && x < (obstacle3.x + obstacle3.w) / block_size) &&
        (y >= obstacle3.y / block_size && y < (obstacle3.y + obstacle3.h) / block_size)) {
        return true;
    }

    return false;
}

void displayGameOver() {
    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color textColor = {51, 51, 0, 255};
    string gameOverText = "Game OVER !!! Socre :  " + to_string(score);
    font = TTF_OpenFont("arial.ttf", 50); 

    SDL_Surface* GOSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, GOSurface);

    int textW = 300, textH = 100;

    SDL_Rect gameOverRect = {150, 150, textW , textH};
    SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);

    SDL_FreeSurface(GOSurface);
    SDL_DestroyTexture(gameOverTexture);

    SDL_RenderPresent(renderer);

    SDL_Delay(1000);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    exit(0);
}
