#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>

const int WindowWidth = 320;
const int WindowHeight = 480;
const int GridWidth = 10;
const int GridHeight = 20;
const int BlockSize = 24;

int grid[GridHeight][GridWidth] = {0};

class Tetromino {
public:
    enum class Shape {
        I, J, L, O, S, T, Z
    };

    Tetromino(Shape shape)
        : shape(shape), x(GridWidth / 2 - 2), y(0) {
        setShape();
    }

    void setShape() {
        switch (shape) {
            case Shape::I:
                shapeArray[0][1] = 1;
                shapeArray[1][1] = 1;
                shapeArray[2][1] = 1;
                shapeArray[3][1] = 1;
                break;
            case Shape::J:
                shapeArray[0][0] = 1;
                shapeArray[1][0] = 1;
                shapeArray[1][1] = 1;
                shapeArray[1][2] = 1;
                break;
            case Shape::L:
                shapeArray[0][2] = 1;
                shapeArray[1][0] = 1;
                shapeArray[1][1] = 1;
                shapeArray[1][2] = 1;
                break;
            case Shape::O:
                shapeArray[0][0] = 1;
                shapeArray[0][1] = 1;
                shapeArray[1][0] = 1;
                shapeArray[1][1] = 1;
                break;
            case Shape::S:
                shapeArray[0][1] = 1;
                shapeArray[0][2] = 1;
                shapeArray[1][0] = 1;
                shapeArray[1][1] = 1;
                break;
            case Shape::T:
                shapeArray[0][1] = 1;
                shapeArray[1][0] = 1;
                shapeArray[1][1] = 1;
                shapeArray[1][2] = 1;
                break;
            case Shape::Z:
                shapeArray[0][0] = 1;
                shapeArray[0][1] = 1;
                shapeArray[1][1] = 1;
                shapeArray[1][2] = 1;
                break;
        }
    }

    const int(*getShape() const)[4] {
        return shapeArray;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    void move(int offsetX, int offsetY) {
        x += offsetX;
        y += offsetY;
    }

    void rotateClockwise() {
        int rotatedShapeArray[4][4];

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                rotatedShapeArray[row][col] = shapeArray[3 - col][row];
            }
        }

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                shapeArray[row][col] = rotatedShapeArray[row][col];
            }
        }
    }

    void rotateCounterClockwise() {
        int rotatedShapeArray[4][4];

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                rotatedShapeArray[row][col] = shapeArray[col][3 - row];
            }
        }

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                shapeArray[row][col] = rotatedShapeArray[row][col];
            }
        }
    }

    bool isColliding(const int gridArray[GridHeight][GridWidth]) const {
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                if (shapeArray[row][col] != 0) {
                    int blockX = x + col;
                    int blockY = y + row;

                    if (blockX < 0 || blockX >= GridWidth || blockY >= GridHeight || gridArray[blockY][blockX] != 0) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

private:
    Shape shape;
    int x;
    int y;
    int shapeArray[4][4] = {{0}};
};

void processInput(sf::RenderWindow& window, Tetromino& tetromino) {
    sf::Event event;

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) {
                tetromino.move(-1, 0);
                if (tetromino.isColliding(grid)) {
                    tetromino.move(1, 0);
                }
            } else if (event.key.code == sf::Keyboard::Right) {
                tetromino.move(1, 0);
                if (tetromino.isColliding(grid)) {
                    tetromino.move(-1, 0);
                }
            } else if (event.key.code == sf::Keyboard::Down) {
                tetromino.move(0, 1);
                if (tetromino.isColliding(grid)) {
                    tetromino.move(0, -1);
                }
            } else if (event.key.code == sf::Keyboard::Up) {
                tetromino.rotateClockwise();
                if (tetromino.isColliding(grid)) {
                    tetromino.rotateCounterClockwise();
                }
            }
        }
    }
}

void update(Tetromino& tetromino, bool& isGameOver, sf::Clock& clock, float& fallTime) {
    float currentTime = clock.getElapsedTime().asSeconds();
    float desiredFallTime = 0.5f; // Adjust this value to control the falling speed

    if (currentTime - fallTime >= desiredFallTime) {
        tetromino.move(0, 1);
        if (tetromino.isColliding(grid)) {
            tetromino.move(0, -1);
            if (tetromino.getY() == 0) {
                isGameOver = true;
            } else {
                // Lock the tetromino into the grid
                for (int row = 0; row < 4; ++row) {
                    for (int col = 0; col < 4; ++col) {
                        if (tetromino.getShape()[row][col] != 0) {
                            int blockX = tetromino.getX() + col;
                            int blockY = tetromino.getY() + row;
                            grid[blockY][blockX] = 1;
                        }
                    }
                }
            }
        }

        fallTime = currentTime;
    }
}

void draw(sf::RenderWindow& window, const Tetromino& tetromino) {
    window.clear(sf::Color::Black);

    for (int row = 0; row < GridHeight; ++row) {
        for (int col = 0; col < GridWidth; ++col) {
            if (grid[row][col] == 1) {
                sf::RectangleShape block(sf::Vector2f(BlockSize, BlockSize));
                block.setFillColor(sf::Color::White);
                block.setPosition(col * BlockSize, row * BlockSize);
                window.draw(block);
            }
        }
    }

    const int(*shapeArray)[4] = tetromino.getShape();

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (shapeArray[row][col] != 0) {
                sf::RectangleShape block(sf::Vector2f(BlockSize, BlockSize));
                block.setFillColor(sf::Color::White);
                block.setPosition((tetromino.getX() + col) * BlockSize, (tetromino.getY() + row) * BlockSize);
                window.draw(block);
            }
        }
    }

    window.display();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "Tetris");
    window.setFramerateLimit(60);

    Tetromino tetromino(Tetromino::Shape::I);

    bool isGameOver = false;
    sf::Clock clock;
    float fallTime = 0.0f;

    while (window.isOpen()) {
        processInput(window, tetromino);

        if (!isGameOver) {
            update(tetromino, isGameOver, clock, fallTime);
        }

        draw(window, tetromino);
    }

    return 0;
}
