#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <list>

//cell size in pixels
const int cell_size = 10;

//timer (ticks) value to slowdown simulation speed (higher timer value --> slower simulation)
const int UPDATE_RATE = 5;

//screen size
const int WIDTH = 1080;
const int HEIGHT = 720;

float random_number(float min, float max) {

    //random variable
    static std::random_device rd;
    static std::default_random_engine eng(rd());
    std::uniform_real_distribution<float> distr(min, max);

    return distr(eng);
}

void create_grid(sf::RenderWindow &window, int cell_size) {

    const int number_lines_vertical = WIDTH / cell_size;
    const int number_lines_horizontal = HEIGHT / cell_size;

    //vertical
    for (int i = 0; i <= number_lines_vertical; i++) {


        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(static_cast<float>(i * cell_size), 0),sf::Color(63, 63, 63, 255)),
            sf::Vertex(sf::Vector2f(static_cast<float>(i * cell_size), HEIGHT),sf::Color(63, 63, 63, 255))
        };

        window.draw(line, 2, sf::Lines);

    }

    //horizontal
    for (int i = 0; i <= number_lines_horizontal; i++) {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(0, static_cast<float>(i * cell_size)),sf::Color(63, 63, 63, 255)),
            sf::Vertex(sf::Vector2f(WIDTH,static_cast<float>(i * cell_size)),sf::Color(63, 63, 63, 255))
        };


        window.draw(line, 2, sf::Lines);

    }


} 

//cells_empty --> returns true if the matrix cell is all dead
bool cells_empty(int cells[(int)(HEIGHT / cell_size)][(int)(WIDTH / cell_size)]) {

    for (int i = 0; i < (int)(HEIGHT / cell_size); i++) {
        for (int j = 0; j < (int)(WIDTH / cell_size); j++) {
            if (cells[i][j] == 1) {
                return false;
            }
        }
    }

    return true;
}

void draw_cells(int cells[(int)(HEIGHT / cell_size)][(int)(WIDTH / cell_size)], int cell_size, sf::RenderWindow& window) {
    for (int i = 0; i < (int)(HEIGHT / cell_size); i++) {
        for (int j = 0; j < (int)(WIDTH / cell_size); j++) {
            if (cells[i][j] == 1) {
                //paint it
                sf::RectangleShape rectangle;
                rectangle.setSize(sf::Vector2f(cell_size, cell_size));
                rectangle.setFillColor(sf::Color::White);
                rectangle.setPosition(j*cell_size, i*cell_size);
                window.draw(rectangle);
            }
            else {
                //clear it
                sf::RectangleShape rectangle;
                rectangle.setSize(sf::Vector2f(cell_size, cell_size));
                rectangle.setFillColor(sf::Color::Black);
                rectangle.setPosition(j * cell_size, i * cell_size);
                window.draw(rectangle);
            }
            
        }
    }
}

//returns the border on which the cell is
std::string is_in_border(int i, int j) {
    std::string response;

    if (i == 0) {
        response = "top";
    }
    else if(i == (int)(HEIGHT / cell_size) - 1) {
        response = "bottom";
    }
    else if (j == 0) {
        response = "left";
    }
    else if (j == (int)(WIDTH / cell_size) - 1) {
        response = "right";
    }
    else {
        response = "none";
    }

    return response;
}

//returns the corner on which the cell is
std::string  is_in_corner(int i, int j) {
    std::string response;

    if (i == 0 && j == 0) {
        response = "top_left";
    }
    else if (i == 0 && j == (int)(WIDTH / cell_size) - 1) {
        response = "top_right";
    }
    else if (i == (int)(HEIGHT / cell_size) - 1 && j == 0) {
        response = "bottom_left";
    }
    else if (i == (int)(HEIGHT / cell_size) - 1 && j == (int)(WIDTH / cell_size) - 1) {
        response = "bottom_right";
    }
    else {
        response = "none";
    }

    return response;
}

//return number of neighbours of the current cell
int number_of_neighbours(int i, int j, int cells[(int)(HEIGHT / cell_size)][(int)(WIDTH / cell_size)]) {
    int neighbours = 0;

    if (cells[i - 1][j - 1] == 1) {
        neighbours++;
    }
    if (cells[i - 1][j] == 1) {
        neighbours++;
    }
    if (cells[i - 1][j+1] == 1) {
        neighbours++;
    }
    if (cells[i][j - 1] == 1) {
        neighbours++;
    }
    if (cells[i][j + 1] == 1) {
        neighbours++;
    }
    if (cells[i+1][j-1] == 1) {
        neighbours++;
    }
    if (cells[i+1][j] == 1) {
        neighbours++;
    }
    if (cells[i+1][j+1] == 1) {
        neighbours++;
    }

    return neighbours;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Conway's game of life");
    
    sf::Clock clock;
    float dt;

    //timer to adjust the simulation speed 
    int timer = UPDATE_RATE;

    bool start_simulation = false;
   
    //cells matrix: 1 -> lives , 0 -> dead
    int cells[(int)(HEIGHT/cell_size)][(int)(WIDTH / cell_size)] = {0};

    int cells_copy[(int)(HEIGHT / cell_size)][(int)(WIDTH / cell_size)] = {0};

    //button to start simulation
    const int btn_x = WIDTH - 60;
    const int btn_y = 10;
    sf::RectangleShape button;
    button.setSize(sf::Vector2f(50, 50));
    button.setOutlineColor(sf::Color::Red);
    button.setOutlineThickness(3);
    button.setPosition(btn_x, btn_y);


    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if(event.type == sf::Event::MouseButtonPressed){
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if ((btn_x < event.mouseButton.x && event.mouseButton.x < btn_x+50) && (10 < event.mouseButton.y && event.mouseButton.y < 50)&&(!start_simulation)) {
                        //start simulation
                        if (!start_simulation) {
                            std::cout << "Animation Started!" << std::endl;
                            start_simulation = !start_simulation;
                        }  

                    }
                    else if(!start_simulation) {
                        bool repeated = false;
                        //let's create a cell only if a cells does not exist already there
                        if (!cells_empty(cells)) {

                            if (cells[(int)(event.mouseButton.y / cell_size)][(int)(event.mouseButton.x / cell_size)] == 1) {
                                std::cout << "You already created that cell!" << std::endl;
                                repeated = true;
                            }

                            if (!repeated) {
                                // get global mouse position
                                std::cout << "mouse x:" << (int)(event.mouseButton.x / cell_size) << std::endl;
                                std::cout << "mouse y:" << (int)(event.mouseButton.y / cell_size) << std::endl << "---------" << std::endl;
                                cells[(int)(event.mouseButton.y / cell_size)][(int)(event.mouseButton.x / cell_size)] = 1;
                            }
                        }
                        else {
                            ;
                            // get global mouse position
                            std::cout << "mouse x:" << (int)(event.mouseButton.x / cell_size) << std::endl;
                            std::cout << "mouse y:" << (int)(event.mouseButton.y / cell_size) << std::endl << "---------" << std::endl;
                            cells[(int)(event.mouseButton.y / cell_size)][(int)(event.mouseButton.x / cell_size)] = 1;
                        }
                    }
                }
            }
        }

        dt = clock.restart().asSeconds();



        //Logic goes here
        if(start_simulation){

            if (!cells_empty(cells) && timer == 0) {

                //cells is copied into cells_copy
                std::copy(&cells[0][0], &cells[0][0]+ (int)(HEIGHT / cell_size) * (int)(WIDTH / cell_size), &cells_copy[0][0]);

                int neighbours = 0;

                //-----------------
                //START GAME RULES
                //-----------------

                for (int i = 0; i < (int)(HEIGHT / cell_size); i++) {
                    for (int j = 0; j < (int)(WIDTH / cell_size); j++) {
                        //cells is going to be modified
                        // border of the canvas is not included in the algorithm

                        if ((0 < i < HEIGHT - 1) && (0 < j < WIDTH - 1)) { //logic is not applied for borders

                            //Any live cell with fewer than two live neighbours dies, as if by underpopulation.
                            if (cells_copy[i][j] == 1 && number_of_neighbours(i, j, cells_copy) < 2) {
                                cells[i][j] = 0;
                            }

                            //Any live cell with two or three live neighbours lives on to the next generation.
                            if (cells_copy[i][j] == 1 && ((number_of_neighbours(i, j, cells_copy) == 2)||(number_of_neighbours(i, j, cells_copy) == 3))) {
                                cells[i][j] = 1;
                            }

                            //Any live cell with more than three live neighbours dies, as if by overpopulation.
                            if (cells_copy[i][j] == 1 && number_of_neighbours(i, j, cells_copy) > 3) {
                                cells[i][j] = 0;
                            }

                            //Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
                            if (cells_copy[i][j] == 0 && number_of_neighbours(i, j, cells_copy) == 3) {
                                cells[i][j] = 1;
                            }

                        }

                        

                        
                    }
                }

                timer = UPDATE_RATE;
            }
            else if (cells_empty(cells) && timer == 0) {
                timer = UPDATE_RATE;
            }

            timer -= 1;
        }
        

        
        //DRAW_________________
        window.clear();


        //Draw all the cells in the vector
        if (!cells_empty(cells)) {
            draw_cells(cells, cell_size, window);
        }

        //grid
        create_grid(window, cell_size);

        //button
        window.draw(button);

        window.display();
        //_____________________
        
        
    }

    return 0;
}