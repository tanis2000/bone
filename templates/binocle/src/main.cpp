//
//  Binocle
//  Copyright(C)2015-2017 Valerio Santinelli
//

#include <SDL.h>
#include <stdio.h>
#include <Binocle/Core/Config.hpp>
#include <Binocle/Core/Log.hpp>
#include <Binocle/Core/Window.hpp>
#include "MyGame.hpp"
//#include "TestBunnyMark.hpp"

using namespace Binocle;
using namespace UberGame;

//Screen dimension constants
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

int main( int argc, char* args[] )
{
    try
    {
        /*Config::load("config.ini");

        if (Config::getBool("debug", false))
        {
            Log::debugMode(true);
            Log::verboseMode(true);
        }*/
        //SDL_Log("Pre-init");
        SDL::init();
        SDL_Log("Post-init");

        int width  = SCREEN_WIDTH; //Config::getInt("width",  640);
        int height = SCREEN_HEIGHT; //Config::getInt("height", 480);

        Window window(width, height, "Binocle Test Game");
        window.setBackgroundColor(Color("white"));

        MyGame *game = new MyGame(&window);
        //TestBunnyMark *game = new TestBunnyMark(&window);

        SDL_Log("Pre game->Run()");
        game->Run(); // main loop of the game

        SDL::exit();
    }
    catch (char *exception)
    {
        // I dont really have a nice exception-handling scheme right
        // now. I must learn how to properly deal with them.
        // As of now, everything that fucks up gets the program
        // terminated.
        Log::error("Exception caught! Quitting...");
        Log::error(exception);
        return -1;
    }
    return 0;

}
