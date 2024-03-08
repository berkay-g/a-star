#include "App.h"

#include "aStar.h"

void App::Setup()
{
    grid = make_grid(rows, width);
    rendra = renderer;
}

void App::Update(SDL_Event &event, bool &quit, float deltaTime)
{
    while (SDL_PollEvent(&event))
    {
        // ImGui_ImplSDL3_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            quit = 1;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                quit = 1;

            // else if (event.key.keysym.sym == SDLK_k)
            //     SDL_Log("fps = %f", 1 / deltaTime);

            else if (event.key.keysym.sym == SDLK_r)
            {
                start = NULL;
                end = NULL;
                grid = make_grid(rows, width);
            }

            keyEvent(event);
            break;
        case SDL_MOUSEMOTION:
            if (event.motion.x < 0 || event.motion.y < 0 || event.motion.x >= 800 || event.motion.y >= 800)
                break;
            
        case SDL_MOUSEBUTTONDOWN:
            mouseEvent(event);
            break;
        }
    }
}

void App::Draw()
{
    // ImGuiIO &io = this->ImguiNewFrame();
    // this->SetRenderDrawColor({22, 22, 22});
    SDL_RenderClear(renderer);

    draw_nodes(renderer, grid);
    draw_grid(renderer, rows, width);

    // this->ImguiRender();
    SDL_RenderPresent(renderer);
}