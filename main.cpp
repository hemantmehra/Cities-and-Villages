#include <iostream>
#include <vector>
#include <raylib.h>

#define POS(x, y) (x * cols + y)

const int width = 1280;
const int height = 720;
const int rows = 10;
const int cols = 10;

enum CellType {
    Empty,
    Capital,
    City,
    Village
};

struct Cell {
    CellType type;
};

enum WorkType {
    Create,
    Upgrade
};

enum BuildingType {
    Farm,
    Market
};

struct Work {
    int cell_x;
    int cell_y;
    WorkType work;
    BuildingType building;
    int remaining_production;

    void set_production_required() {
        if (work == Create && Farm) {
            remaining_production = 4;
        }

        else if (work == Upgrade && Farm) {
            remaining_production = 5;
        }

        else if (work == Create && Market) {
            remaining_production = 6;
        }

        else if (work == Upgrade && Market) {
            remaining_production = 7;
        }

        else remaining_production = 0;
    }
};

struct Civ {
    int population;
    int food;
    int gold;
    std::vector<Vector2> cells;
    Work *current_work;
    std::vector<Work *> available_work;

    void set_available_work(Cell *world)
    {
        available_work.clear();
        for(auto cell: cells) {
            int x = cell.x;
            int y = cell.y;

            if (world[POS(x, y)].type == City) {
                Work *work = new Work;
                work->cell_x = x;
                work->cell_y = y;
                work->building = Market;
                work->work = Create;
                work->set_production_required();
                available_work.push_back(work);
            }

            if (world[POS(x, y)].type == Village) {
                Work *work = new Work;
                work->cell_x = x;
                work->cell_y = y;
                work->building = Farm;
                work->work = Create;
                work->set_production_required();
                available_work.push_back(work);
            }

            if (world[POS(x, y)].type == Capital) {
                
            }
        }
    }

    int next_population_food_target()
    {
        return population * 2 + 1;
    }

    void next_turn(Cell *world)
    {
        int food_target = next_population_food_target();
        if (food_target <= food) {
            population++;
        }

        for(auto cell: cells) {
            int x = cell.x;
            int y = cell.y;

            if (world[POS(x, y)].type == City) {
                gold+=2;
            }
            if (world[POS(x, y)].type == Village) {
                food+=4;
            }
            if (world[POS(x, y)].type == Capital) {
                food++;
                gold++;
            }
        }
        // std::cout << "Food :" << food << '\n';
        food -= population;
    }
};

enum SelectType {
    Select_Map,
    Select_Option
};

struct Game {
    int turn;
    SelectType current_selection;
};

std::string get_cell_string(CellType celltype)
{
    switch(celltype) {
        case Capital:
            return "Capital";
        case City:
            return "City";
        case Village:
            return "Village";
        default:
            return "";
    }
}

int main()
{
    InitWindow(width, height, "Cities and Villages");

    const int cell_size = 60;
    const int cell_select_padding = 5;
    const int font_size_default = 24;

    Game game = {0, Select_Map};
    Civ civ;
    Cell cells[100];
    int selected_r = 5;
    int selected_c = 5;
    int selected_option = 0;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            cells[POS(i, j)].type = Empty;
        }
    }

    civ.population = 4;
    civ.food = 4;
    civ.gold = 2;
    civ.current_work = NULL;

    cells[POS(5, 5)].type = Capital;

    civ.cells.push_back((Vector2){5, 5});

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) break;

        if (IsKeyPressed(KEY_ENTER)) {
            if (game.current_selection == Select_Map) {
                game.turn++;
                civ.next_turn(cells);
                if (civ.current_work != NULL) {
                    civ.current_work->remaining_production--;
                    if (civ.current_work->remaining_production == 0) {
                        civ.current_work = NULL;
                    }
                }
            }
            if (game.current_selection == Select_Option) {
                civ.current_work = civ.available_work.at(selected_option);
                game.current_selection = Select_Map;
            }
        }

        if (IsKeyPressed(KEY_C) && cells[POS(selected_r, selected_c)].type == Empty) {
            cells[POS(selected_r, selected_c)].type = City;
            civ.cells.push_back((Vector2){selected_r, selected_c});
            civ.set_available_work(cells);
        }

        if (IsKeyPressed(KEY_V) && cells[POS(selected_r, selected_c)].type == Empty) {
            cells[POS(selected_r, selected_c)].type = Village;
            civ.cells.push_back((Vector2){selected_r, selected_c});
            civ.set_available_work(cells);
        }

        if (IsKeyPressed(KEY_LEFT)) selected_c--;
        if (IsKeyPressed(KEY_RIGHT)) selected_c++;
        if (IsKeyPressed(KEY_UP)) {
            if (game.current_selection == Select_Map) selected_r--;
            if (game.current_selection == Select_Option) {
                selected_option = std::max(0, selected_option - 1);
            }
        }
        if (IsKeyPressed(KEY_DOWN)) {
            if (game.current_selection == Select_Map) selected_r++;
            if (game.current_selection == Select_Option) {
                selected_option = std::min((int)civ.available_work.size() - 1, selected_option + 1);
            }
        }

        if (civ.current_work == NULL && civ.available_work.size() > 0) {
            game.current_selection = Select_Option;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            DrawText(TextFormat("Turn: %d", game.turn), cols * cell_size + 5, 0, font_size_default, ORANGE);
            DrawText(TextFormat("Population: %d", civ.population), cols * cell_size + 5, font_size_default, font_size_default, ORANGE);
            DrawText(TextFormat("Food: %d", civ.food), cols * cell_size + 5, font_size_default*2, font_size_default, ORANGE);
            DrawText(TextFormat("Gold: %d", civ.gold), cols * cell_size + 5, font_size_default*3, font_size_default, ORANGE);

            int c = 0;

            if (civ.current_work != NULL) {
                int x = civ.current_work->cell_x;
                int y = civ.current_work->cell_y;
                Cell cell = cells[POS(x, y)];
                std::string s = get_cell_string(cell.type);
                DrawText(
                    TextFormat("Current: %s [%d %d], Remaining: %d", s.c_str(), x, y, civ.current_work->remaining_production),
                    cols * cell_size + 10,
                    100 + font_size_default * c,
                    font_size_default,
                    GREEN
                );
            }

            for(auto work : civ.available_work) {
                int x = work->cell_x;
                int y = work->cell_y;
                Cell cell = cells[POS(x, y)];
                std::string s = get_cell_string(cell.type);
                DrawText(
                    TextFormat("%s [%d, %d]", s.c_str(), x, y),
                    cols * cell_size + 10,
                    150 + font_size_default * c,
                    font_size_default,
                    WHITE
                );
                if (selected_option == c && game.current_selection == Select_Option)
                DrawRectangle(cols * cell_size + 5, 150 + font_size_default * c, 5, font_size_default, RED);
                c++;
            }

            for (int i=0; i<rows; i++) {
                for (int j=0; j<cols; j++) {
                    DrawRectangleLines(j*cell_size, i*cell_size, cell_size, cell_size, GRAY);
                    
                    if (i == selected_r && j == selected_c && game.current_selection == Select_Map) {
                        DrawRectangleLines(
                            j*cell_size+cell_select_padding,
                            i*cell_size+cell_select_padding,
                            cell_size-2*cell_select_padding,
                            cell_size-2*cell_select_padding,
                            RED
                        );
                    }

                    if (cells[POS(i, j)].type == Capital) {
                        DrawCircleLines(
                            j*cell_size + cell_size/2,
                            i*cell_size + cell_size/2,
                            cell_size/3,
                            GOLD
                        );
                    }

                    if (cells[POS(i, j)].type == City) {
                        DrawText("C", j*cell_size + 5, i*cell_size + 5, 16, BLUE);
                    }

                    if (cells[POS(i, j)].type == Village) {
                        DrawText("V", j*cell_size + 5, i*cell_size + 5, 16, YELLOW);
                    }
                }
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
