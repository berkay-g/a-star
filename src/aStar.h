#pragma once

#include "Color.h"
#include "SDL_render.h"

#include <vector>
#include <limits>
#include <algorithm>
#include <queue>
#include <set>
#include <tuple>
#include <functional>
#include <unordered_map>

class Node
{
public:
    int x, y;
    int row, col;
    Color color;
    std::vector<Node*> neighbors;
    int width;
    int total_rows;

    float f_score = std::numeric_limits<float>::infinity();
    float g_score = std::numeric_limits<float>::infinity();

    Node(int row, int col, int width, int total_rows)
        : row(row), col(col), width(width), total_rows(total_rows)
    {
        x = row * width;
        y = col * width;
        color = Color::White;
    }

    const bool is_closed()
    {
        return color == Color::Red;
    }

    const bool is_open()
    {
        return color == Color::Green;
    }

    const bool is_barrier()
    {
        return color == Color::Black;
    }

    const bool is_start()
    {
        return color == Color::Yellow;
    }

    const bool is_end()
    {
        return color == Color::Cyan;
    }

    void reset()
    {
        color = Color::White;
    }

    void make_start()
    {
        color = Color::Yellow;
    }

    void make_closed()
    {
        color = Color::Red;
    }

    void make_open()
    {
        color = Color::Green;
    }

    void make_barrier()
    {
        color = Color::Black;
    }

    void make_end()
    {
        color = Color::Cyan;
    }

    void make_path()
    {
        color = Color::Magenta;
    }

    void update_neighbors(std::vector<std::vector<Node>> &grid)
    {
        neighbors.clear();
        if (row < total_rows - 1 && !grid[row + 1][col].is_barrier()) // down
            neighbors.push_back(&grid[row + 1][col]);

        if (row > 0 && !grid[row - 1][col].is_barrier()) // up
            neighbors.push_back(&grid[row - 1][col]);

        if (col < total_rows - 1 && !grid[row][col + 1].is_barrier()) // right
            neighbors.push_back(&grid[row][col + 1]);

        if (col > 0 && !grid[row][col - 1].is_barrier()) // left
            neighbors.push_back(&grid[row][col - 1]);
    }

    bool operator<(const Node &other)
    {
        return false;
    }

    bool operator==(const Node &other) const
    {
        return (x == other.x) && (y == other.y) && (row == other.row) && (col == other.col) && (color == other.color) && (neighbors == other.neighbors) && (width == other.width) && (total_rows == other.total_rows);
    }
};

int rows = 50;
int width = 800;
std::vector<std::vector<Node>> grid;
bool started = false;
Node *start = NULL, *end = NULL;
SDL_Renderer *rendra;

void draw_node(SDL_Renderer *renderer, Node &node)
{
    SDL_SetRenderDrawColor(renderer, node.color.r, node.color.g, node.color.b, node.color.a);
    SDL_Rect rect = {node.x, node.y, node.width, node.width};
    SDL_RenderFillRect(renderer, &rect);
}

int h(int x1, int y1, int x2, int y2)
{
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

std::vector<std::vector<Node>> make_grid(int rows, int width)
{
    std::vector<std::vector<Node>> grid;
    int gap = (int)width / rows;

    for (int i = 0; i < rows; i++)
    {
        grid.push_back({});
        for (int j = 0; j < rows; j++)
        {
            grid[i].push_back(Node(i, j, gap, rows));
        }
    }
    return grid;
}

void draw_grid(SDL_Renderer *renderer, int rows, int width)
{
    int gap = (int)width / rows;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i = 0; i < rows; i++)
    {
        SDL_RenderDrawLine(renderer, 0, i * gap, width, i * gap);
        for (int j = 0; j < rows; j++)
        {
            SDL_RenderDrawLine(renderer, j * gap, 0, j * gap, width);
        }
    }
}

std::pair<int, int> get_clicked_pos(int mouseX, int mouseY, int rows, int width)
{
    int gap = (int)width / rows;
    int col = (int)mouseY / gap;
    int row = (int)mouseX / gap;

    return {row, col};
}

void draw_nodes(SDL_Renderer *renderer, std::vector<std::vector<Node>> &grid)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
        {
            draw_node(renderer, grid[i][j]);
        }
    }
}

void reconstruct_path(std::unordered_map<Node*, Node*>& came_from, Node* current)
{
    for (size_t i = 0; i < came_from.size(); i++)
    {
        if (current == start)
            return;
        current = came_from[current];
        current->make_path();
        SDL_RenderClear(rendra);
        draw_nodes(rendra, grid);
        draw_grid(rendra, rows, width);
        SDL_RenderPresent(rendra);
    }
}

void algorithm()
{
    int count = 0;

    std::priority_queue<std::tuple<float, int, Node *>, std::vector<std::tuple<float, int, Node *>>,
                        std::function<bool(const std::tuple<float, int, Node *> &, const std::tuple<float, int, Node *> &)>>
        open_set(
            [](const std::tuple<float, int, Node *> &a, const std::tuple<float, int, Node *> &b)
            {
                return std::get<0>(a) > std::get<0>(b); // Compare f_scores in ascending order
            });

    open_set.emplace(0.f, count, start);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
        {
            grid[i][j].g_score = std::numeric_limits<float>::infinity();
            grid[i][j].f_score = std::numeric_limits<float>::infinity();
        }
    }
    start->g_score = 0.f;
    start->f_score = (float)h(start->row, start->col, end->row, end->col);

    std::unordered_map<Node*, Node*> came_from;

    std::set<Node *> open_set_hash = {start};

    SDL_Event event;
    while (!open_set.empty())
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                started = false;
                return;
            }
        }

        auto current = std::get<2>(open_set.top());
        open_set.pop();

        open_set_hash.erase(current);

        if (current == end)
        {
            reconstruct_path(came_from, end);
            end->make_end();
            start->make_start();
            started = false;
            return;
        }

        for (auto &neighbor : current->neighbors)
        {
            auto temp_g_score = current->g_score + 1;

            if (temp_g_score < neighbor->g_score)
            {
                // camefrom
                came_from[neighbor] = current;
                neighbor->g_score = temp_g_score;
                neighbor->f_score = temp_g_score + h(neighbor->row, neighbor->col, end->row, end->col);

                if (open_set_hash.find(neighbor) == open_set_hash.end())
                {
                    count += 1;
                    open_set.emplace(neighbor->f_score, count, neighbor);
                    open_set_hash.emplace(neighbor);
                    neighbor->make_open();
                }
            }
        }

        SDL_RenderClear(rendra);
        draw_nodes(rendra, grid);
        draw_grid(rendra, rows, width);
        SDL_RenderPresent(rendra);

        if (current != start)
            current->make_closed();

    }

    started = false;
}

void mouseEvent(SDL_Event event)
{
    if (started)
        return;

    if (event.button.button == SDL_BUTTON_LMASK || event.button.button == SDL_BUTTON_LEFT)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        std::pair<int, int> rowcol = get_clicked_pos(mouseX, mouseY, rows, width);
        Node *node = &grid[rowcol.first][rowcol.second];
        if (!start)
        {
            start = node;
            start->make_start();
        }
        else if (!end && node != start)
        {
            end = node;
            end->make_end();
        }
        else if (node != start && node != end)
        {
            node->make_barrier();
        }
    }
    else if (event.button.button == SDL_BUTTON_RMASK || event.button.button == SDL_BUTTON_RIGHT)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        std::pair<int, int> rowcol = get_clicked_pos(mouseX, mouseY, rows, width);
        Node *node = &grid[rowcol.first][rowcol.second];

        node->reset();
        if (node == start && node != end)
            start = NULL;
        else if (node == end && node != start)
            end = NULL;
    }
}

void keyEvent(SDL_Event event)
{
    if (event.key.keysym.sym == SDLK_SPACE && !started && start && end)
    {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < grid[i].size(); j++)
                grid[i][j].update_neighbors(grid);
        started = true;
        algorithm();
    }
}