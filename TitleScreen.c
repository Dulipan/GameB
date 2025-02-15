// Filename: TitleScreen.c
// Contains code for the functions that are specific to the title screen.
//
// Project Codename: GameB
// TODO: Come up with a better name later.
// 2020 Joseph Ryan Ries <ryanries09@gmail.com>
// My YouTube series where we program an entire video game from scratch in C.
// Watch it on YouTube:    https://www.youtube.com/watch?v=3zFFrBSdBvA
// Follow along on GitHub: https://github.com/ryanries/GameB
// Find me on Twitter @JosephRyanRies 
// # License
// ----------
// The source code in this project is licensed under the MIT license.
// The media assets such as artwork, custom fonts, music and sound effects are licensed under a separate license.
// A copy of that license can be found in the 'Assets' directory.
// stb_vorbis by Sean Barrett is public domain and a copy of its license can be found in the stb_vorbis.c file.

#include "Main.h"

#include "TitleScreen.h"

MENUITEM gMI_ResumeGame = { "Resume", (GAME_RES_WIDTH / 2) - ((6 * 6) / 2), 100, FALSE, MenuItem_TitleScreen_Resume };

MENUITEM gMI_StartNewGame = { "Start New Game", (GAME_RES_WIDTH / 2) - ((14 * 6) / 2), 115, TRUE, MenuItem_TitleScreen_StartNew };

MENUITEM gMI_LoadSavedGame = { "Load Saved Game", (GAME_RES_WIDTH / 2) - ((15 * 6) / 2), 130, TRUE, MenuItem_TitleScreen_LoadSavedGame };

MENUITEM gMI_Options = { "Options", (GAME_RES_WIDTH / 2) - ((7 * 6) / 2), 145, TRUE, MenuItem_TitleScreen_Options };

MENUITEM gMI_Exit = { "Exit", (GAME_RES_WIDTH / 2) - ((4 * 6) / 2), 160, TRUE, MenuItem_TitleScreen_Exit };

MENUITEM* gMI_TitleScreenItems[] = { &gMI_ResumeGame, &gMI_StartNewGame, &gMI_LoadSavedGame, &gMI_Options, &gMI_Exit };

MENU gMenu_TitleScreen = { "Title Screen", 1, _countof(gMI_TitleScreenItems), gMI_TitleScreenItems };

void DrawTitleScreen(void)
{
    static uint64_t LocalFrameCounter;

    static uint64_t LastFrameSeen = 0;
    
    static PIXEL32 TextColor;

    // If global TotalFramesRendered is greater than LastFrameSeen,
    // that means we have either just entered this gamestate for the first time,
    // or we have left this gamestate previously and have just come back to it.
    // For example we may have gone from the title screen, to the options screen,
    // and then back to the title screen again. In that case, we want to reset all
    // of the "local state," i.e., things that are local to this game state. Such
    // as text animation, selected menu item, etc.
    if (gPerformanceData.TotalFramesRendered > (LastFrameSeen + 1))
    {
        LocalFrameCounter = 0;

        if (gPlayer.Active == TRUE)
        {
            gMenu_TitleScreen.SelectedItem = 0;

            gMI_ResumeGame.Enabled = TRUE;
        }
        else
        {
            gMenu_TitleScreen.SelectedItem = 1;

            gMI_ResumeGame.Enabled = FALSE;
        }

        gInputEnabled = FALSE;
    }

    memset(gBackBuffer.Memory, 0, GAME_DRAWING_AREA_MEMORY_SIZE);

    ApplyFadeIn(LocalFrameCounter, COLOR_NES_WHITE, &TextColor, NULL);    

    //    AARRGGBB ?
    //__stosd(gBackBuffer.Memory, 0xFF0000FF, GAME_DRAWING_AREA_MEMORY_SIZE / sizeof(PIXEL32));

    BlitStringToBuffer(GAME_NAME, &g6x7Font, &TextColor, (GAME_RES_WIDTH / 2) - ((uint16_t)(strlen(GAME_NAME) * 6) / 2), 60);

    for (uint8_t MenuItem = 0; MenuItem < gMenu_TitleScreen.ItemCount; MenuItem++)
    {
        if (gMenu_TitleScreen.Items[MenuItem]->Enabled == TRUE)
        {
            BlitStringToBuffer(gMenu_TitleScreen.Items[MenuItem]->Name,
                &g6x7Font,
                &TextColor,
                gMenu_TitleScreen.Items[MenuItem]->x,
                gMenu_TitleScreen.Items[MenuItem]->y);

        }
    }

    BlitStringToBuffer("\xBB",
        &g6x7Font,
        &TextColor,
        gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->x - 6,
        gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->y);

    LocalFrameCounter++;

    LastFrameSeen = gPerformanceData.TotalFramesRendered;
}

void PPI_TitleScreen(void)
{
    if (gGameInput.DownKeyIsDown && !gGameInput.DownKeyWasDown)
    {
        if (gMenu_TitleScreen.SelectedItem < gMenu_TitleScreen.ItemCount - 1)
        {
            gMenu_TitleScreen.SelectedItem++;            
        }
        else
        {
            // Wrap around to the Resume button, if it is available.
            // Otherwise, wrap around to the Start New Game button.

            if (gPlayer.Active == TRUE)
            {                
                gMenu_TitleScreen.SelectedItem = 0;
            }
            else
            {
                gMenu_TitleScreen.SelectedItem = 1;
            }
        }

        PlayGameSound(&gSoundMenuNavigate);
    }

    if (gGameInput.UpKeyIsDown && !gGameInput.UpKeyWasDown)
    {
        // If we're at the top of the menu, wrap around to the bottom.
        // The Resume button only appears if a game is in progress.        

        if (gMenu_TitleScreen.SelectedItem == 0)
        {
            ASSERT(gPlayer.Active == TRUE, "Resume button is selected, yet no game is currently in progress!?");

            gMenu_TitleScreen.SelectedItem = gMenu_TitleScreen.ItemCount - 1;
        }
        else if (gMenu_TitleScreen.SelectedItem == 1)
        {
            if (gPlayer.Active == TRUE)
            {
                gMenu_TitleScreen.SelectedItem--;
            }
            else
            {
                gMenu_TitleScreen.SelectedItem = gMenu_TitleScreen.ItemCount - 1;
            }
        }
        else
        {
            gMenu_TitleScreen.SelectedItem--;
        }

        PlayGameSound(&gSoundMenuNavigate);
    }

    if (gGameInput.ChooseKeyIsDown && !gGameInput.ChooseKeyWasDown)
    {
        gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->Action();

        PlayGameSound(&gSoundMenuChoose);
    }
}

void MenuItem_TitleScreen_Resume(void)
{
    ASSERT(gCurrentGameState == GAMESTATE_TITLESCREEN, "Invalid game state!");

    gPreviousGameState = gCurrentGameState;

    gCurrentGameState = GAMESTATE_OVERWORLD;

    LogMessageA(LL_INFO, "[%s] Transitioning from game state %d to %d. Player selected '%s' from '%s' menu.",
        __FUNCTION__,
        gPreviousGameState,
        gCurrentGameState,
        gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->Name,
        gMenu_TitleScreen.Name);
}


// If a game is already in progress, this should prompt the user if they are sure 
// they want to start a new game first, and lose any unsaved progress.
// Otherwise, just go directly to the character naming screen.

void MenuItem_TitleScreen_StartNew(void)
{
    ASSERT(gCurrentGameState == GAMESTATE_TITLESCREEN, "Invalid game state!");

    if (gPlayer.Active == TRUE)
    {
        gPreviousGameState = gCurrentGameState;

        gCurrentGameState = GAMESTATE_NEWGAMEAREYOUSURE;        
    }
    else
    {
        gPreviousGameState = gCurrentGameState;

        gCurrentGameState = GAMESTATE_CHARACTERNAMING;
    }

    LogMessageA(LL_INFO, "[%s] Transitioning from game state %d to %d. Player selected '%s' from '%s' menu.",
        __FUNCTION__,
        gPreviousGameState,
        gCurrentGameState,
        gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->Name,
        gMenu_TitleScreen.Name);
}

void MenuItem_TitleScreen_LoadSavedGame(void)
{

}

void MenuItem_TitleScreen_Options(void)
{
    ASSERT(gCurrentGameState == GAMESTATE_TITLESCREEN, "Invalid game state!");

    gPreviousGameState = gCurrentGameState;

    gCurrentGameState = GAMESTATE_OPTIONSSCREEN;

    LogMessageA(LL_INFO, "[%s] Transitioning from game state %d to %d. Player selected '%s' from '%s' menu.", 
        __FUNCTION__, 
        gPreviousGameState, 
        gCurrentGameState, 
        gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->Name,
        gMenu_TitleScreen.Name);
}

void MenuItem_TitleScreen_Exit(void)
{
    ASSERT(gCurrentGameState == GAMESTATE_TITLESCREEN, "Invalid game state!");

    gPreviousGameState = gCurrentGameState;

    gCurrentGameState = GAMESTATE_EXITYESNOSCREEN;

    LogMessageA(LL_INFO, "[%s] Transitioning from game state %d to %d. Player selected '%s' from '%s' menu.",
        __FUNCTION__,
        gPreviousGameState,
        gCurrentGameState,
        gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->Name,
        gMenu_TitleScreen.Name);
}