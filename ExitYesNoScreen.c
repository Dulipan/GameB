// Filename: ExitYesNoScreen.c
// Contains code for the functions that are specific to the "Are you sure you want to exit?" screen.
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

#include "ExitYesNoScreen.h"

MENUITEM gMI_ExitYesNo_Yes = { "Yes", (GAME_RES_WIDTH / 2) - ((3 * 6) / 2), 100, TRUE, MenuItem_ExitYesNo_Yes };

MENUITEM gMI_ExitYesNo_No = { "No",   (GAME_RES_WIDTH / 2) - ((2 * 6) / 2), 115, TRUE, MenuItem_ExitYesNo_No };

MENUITEM* gMI_ExitYesNoItems[] = { &gMI_ExitYesNo_Yes, &gMI_ExitYesNo_No };

MENU gMenu_ExitYesNo = { "Are you sure you want to exit?", 1, _countof(gMI_ExitYesNoItems), gMI_ExitYesNoItems };

void DrawExitYesNoScreen(void)
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

        memset(&TextColor, 0, sizeof(PIXEL32));

        gInputEnabled = FALSE;
    }

    memset(gBackBuffer.Memory, 0, GAME_DRAWING_AREA_MEMORY_SIZE);

    ApplyFadeIn(LocalFrameCounter, COLOR_NES_WHITE, &TextColor, NULL);

    BlitStringToBuffer(gMenu_ExitYesNo.Name,
        &g6x7Font,
        &TextColor,
        (GAME_RES_WIDTH / 2) - ((uint16_t)(strlen(gMenu_ExitYesNo.Name) * 6) / 2),
        60);

    BlitStringToBuffer(gMenu_ExitYesNo.Items[0]->Name,
        &g6x7Font,
        &TextColor,
        (GAME_RES_WIDTH / 2) - ((uint16_t)(strlen(gMenu_ExitYesNo.Items[0]->Name) * 6) / 2),
        100);

    BlitStringToBuffer(gMenu_ExitYesNo.Items[1]->Name,
        &g6x7Font,
        &TextColor,
        (GAME_RES_WIDTH / 2) - ((uint16_t)(strlen(gMenu_ExitYesNo.Items[1]->Name) * 6) / 2),
        115);

    BlitStringToBuffer("\xBB",
        &g6x7Font,
        &TextColor,
        gMenu_ExitYesNo.Items[gMenu_ExitYesNo.SelectedItem]->x - 6,
        gMenu_ExitYesNo.Items[gMenu_ExitYesNo.SelectedItem]->y);

    LocalFrameCounter++;

    LastFrameSeen = gPerformanceData.TotalFramesRendered;
}

void PPI_ExitYesNo(void)
{
    if (gGameInput.DownKeyIsDown && !gGameInput.DownKeyWasDown)
    {
        if (gMenu_ExitYesNo.SelectedItem < gMenu_ExitYesNo.ItemCount - 1)
        {
            gMenu_ExitYesNo.SelectedItem++;

            PlayGameSound(&gSoundMenuNavigate);
        }
    }

    if (gGameInput.UpKeyIsDown && !gGameInput.UpKeyWasDown)
    {
        if (gMenu_ExitYesNo.SelectedItem > 0)
        {
            gMenu_ExitYesNo.SelectedItem--;

            PlayGameSound(&gSoundMenuNavigate);
        }
    }

    if (gGameInput.ChooseKeyIsDown && !gGameInput.ChooseKeyWasDown)
    {
        gMenu_ExitYesNo.Items[gMenu_ExitYesNo.SelectedItem]->Action();

        PlayGameSound(&gSoundMenuChoose);
    }
}

void MenuItem_ExitYesNo_Yes(void)
{
    ASSERT(gCurrentGameState == GAMESTATE_EXITYESNOSCREEN, "Invalid game state!");

    LogMessageA(LL_INFO, "[%s] Player chose 'Yes' when asked 'Do you really want to exit the game?'", __FUNCTION__);

    SendMessageA(gGameWindow, WM_CLOSE, 0, 0);
}

void MenuItem_ExitYesNo_No(void)
{
    ASSERT(gCurrentGameState == GAMESTATE_EXITYESNOSCREEN, "Invalid game state!");

    gCurrentGameState = gPreviousGameState;

    gPreviousGameState = GAMESTATE_EXITYESNOSCREEN;

    LogMessageA(LL_INFO, "[%s] Transitioning from game state %d to %d. Player selected '%s' from '%s' menu.",
        __FUNCTION__,
        gPreviousGameState,
        gCurrentGameState,
        gMenu_ExitYesNo.Items[gMenu_ExitYesNo.SelectedItem]->Name,
        gMenu_ExitYesNo.Name);
}