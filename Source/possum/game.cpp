#include "game.h"
#include "entity.h"
#include <math.h>
#include <iostream>

namespace possum {
    Game::Game(): pendingScene(0), currentScene(0)
    {
        //ctor
    }

    Game::~Game()
    {
        //dtor
    }

    void Game::create(int width, int height, std::string title){
        window.create(sf::VideoMode(width, height), title);
        gameState.set("width", width);
        gameState.set("height", height);
    }

    void Game::run(){
        sf::Clock clock;
        while (window.isOpen()){
            if (pendingScene != 0){
                currentScene = pendingScene;
                pendingScene = 0;
            }
            sf::Event event;
            while (window.pollEvent(event)){
                if (event.type == sf::Event::Closed){
                    window.close();
                } else if (event.type == sf::Event::KeyPressed){
                    for (std::vector<std::unique_ptr<Entity> >::iterator i = currentScene->entities.begin(); i < currentScene->entities.end(); ++i){
                        (*i)->handle_event(KEY_DOWN, *currentScene, gameState, &event.key);
                    }
                } else if (event.type == sf::Event::KeyReleased){
                    for (std::vector<std::unique_ptr<Entity> >::iterator i = currentScene->entities.begin(); i < currentScene->entities.end(); ++i){
                        (*i)->handle_event(KEY_UP, *currentScene, gameState, &event.key);
                    }
                } else if (event.type == sf::Event::MouseButtonPressed){
                    for (std::vector<std::unique_ptr<Entity> >::iterator i = currentScene->entities.begin(); i < currentScene->entities.end(); ++i){
                        (*i)->handle_event(BUTTON_DOWN, *currentScene, gameState, &event.mouseButton);
                    }
                } else if (event.type == sf::Event::MouseButtonReleased){
                    for (std::vector<std::unique_ptr<Entity> >::iterator i = currentScene->entities.begin(); i < currentScene->entities.end(); ++i){
                        (*i)->handle_event(BUTTON_UP, *currentScene, gameState, &event.mouseButton);
                    }
                } else if (event.type == sf::Event::MouseMoved){
                    for (std::vector<std::unique_ptr<Entity> >::iterator i = currentScene->entities.begin(); i < currentScene->entities.end(); ++i){
                        (*i)->handle_event(POINTER_MOVE, *currentScene, gameState, &event.mouseMove);
                    }
                } else if (event.type == sf::Event::Resized){
                    std::cout << "Resized to " << event.size.width << "x" << event.size.height << std::endl;
                    gameState.set("width", event.size.width);
                    gameState.set("height", event.size.height);
                }
            }
            sf::Time time = clock.getElapsedTime() + clock.restart();
            for (std::vector<std::unique_ptr<Entity> >::iterator i = currentScene->entities.begin(); i < currentScene->entities.end(); ++i){
                (*i)->handle_event(UPDATE, *currentScene, gameState, &time);
            }
            for (std::vector<std::unique_ptr<Entity> >::iterator ep1 = currentScene->entities.begin(); ep1 < currentScene->entities.end(); ++ep1){
                Entity* e1 = ep1->get();
                for (std::vector<std::unique_ptr<Entity> >::iterator ep2 = currentScene->entities.begin(); ep2 < ep1; ++ep2){
                    Entity* e2 = ep2->get();
                    if (pow(e1->x-e2->x, 2)+pow(e1->y-e2->y, 2) < pow(e1->radius+e2->radius, 2)){
                        e1->handle_event(COLLISION, *currentScene, gameState, &(*e2));
                        e2->handle_event(COLLISION, *currentScene, gameState, &(*e1));
                    }
                }
            }
            sf::View view(sf::FloatRect(gameState.get("x"), gameState.get("y"), gameState.get("width"), gameState.get("height")));
            window.setView(view);
            window.clear(currentScene->background);
            currentScene->entities.erase(std::remove_if(currentScene->entities.begin(), currentScene->entities.end(), [](std::unique_ptr<Entity> const &e){return e->dead;}), currentScene->entities.end());
            for (std::vector<std::unique_ptr<Entity> >::iterator i = currentScene->entities.begin(); i < currentScene->entities.end(); ++i){
                /* if (i->callbacks[REDRAW] == 0){
                    std::cout << "Not ";
                }
                std::cout << "Redrawing object" << i->type << "!" << std::endl;*/
                (*i)->handle_event(REDRAW, *currentScene, gameState, &window);
            }
            window.display();
        }
    }
}
