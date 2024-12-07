// ASCIIpOrtal ver 1.3 by Joseph Larson
// Copyright (c) 2009, 2011 Joseph Larson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// The source links with SDL, PDCurses compiled for SDL rendering (PDCSDL)
// and SDL mixer.

#include <curses.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
using namespace std;

#include "ap_input.h"

#ifndef __NOSDL__
#include "pdcsdl.h"
#include <SDL.h>
#endif
#ifndef __NOSOUND__
#include "ap_sound.h"
#endif

//#include "ap_play.h"
//#include "ap_draw.h"
#include "menu.h"

/*
extern statstype levelstats;
extern int level;
extern int maxlevel;
extern int ticks;
extern int cheatview;
extern vector<vector<int> > map;
*/

#ifndef __NOSDL__
bool blocking = false;
int oldkey = ERR;

static struct
{
  int keycode;
  bool numkeypad;
  unsigned short normal;
  unsigned short shifted;
  unsigned short control;
  unsigned short alt;
} key_table[] = // shamelessly copied.
{
/* keycode  keypad  normal       shifted     control  alt*/
 {SDLK_LEFT,  FALSE,  KEY_LEFT,    KEY_SLEFT,    CTL_LEFT,  ALT_LEFT},
 {SDLK_RIGHT,  FALSE,  KEY_RIGHT,   KEY_SRIGHT,   CTL_RIGHT,  ALT_RIGHT},
 {SDLK_UP,  FALSE,  KEY_UP,      KEY_SUP,     CTL_UP,  ALT_UP},
 {SDLK_DOWN,  FALSE,  KEY_DOWN,    KEY_SDOWN,    CTL_DOWN,  ALT_DOWN},
 {SDLK_HOME,  FALSE,  KEY_HOME,    KEY_SHOME,    CTL_HOME,  ALT_HOME},
 {SDLK_RETURN,  FALSE,  '\n',    '\n',  CTL_PADENTER,ALT_PADENTER},
 {SDLK_END,  FALSE,  KEY_END,     KEY_SEND,     CTL_END,  ALT_END},
 {SDLK_PAGEUP,  FALSE,  KEY_PPAGE,   KEY_SPREVIOUS,CTL_PGUP,  ALT_PGUP},
 {SDLK_PAGEDOWN,FALSE,  KEY_NPAGE,   KEY_SNEXT,    CTL_PGDN,  ALT_PGDN},
 {SDLK_INSERT,  FALSE,  KEY_IC,      KEY_SIC,     CTL_INS,  ALT_INS},
 {SDLK_DELETE,  FALSE,  KEY_DC,      KEY_SDC,     CTL_DEL,  ALT_DEL},
 {SDLK_F1,  FALSE,  KEY_F(1),    KEY_F(13),    KEY_F(25),  KEY_F(37)},
 {SDLK_F2,  FALSE,  KEY_F(2),    KEY_F(14),    KEY_F(26),  KEY_F(38)},
 {SDLK_F3,  FALSE,  KEY_F(3),    KEY_F(15),    KEY_F(27),  KEY_F(39)},
 {SDLK_F4,  FALSE,  KEY_F(4),    KEY_F(16),    KEY_F(28),  KEY_F(40)},
 {SDLK_F5,  FALSE,  KEY_F(5),    KEY_F(17),    KEY_F(29),  KEY_F(41)},
 {SDLK_F6,  FALSE,  KEY_F(6),    KEY_F(18),    KEY_F(30),  KEY_F(42)},
 {SDLK_F7,  FALSE,  KEY_F(7),    KEY_F(19),    KEY_F(31),  KEY_F(43)},
 {SDLK_F8,  FALSE,  KEY_F(8),    KEY_F(20),    KEY_F(32),  KEY_F(44)},
 {SDLK_F9,  FALSE,  KEY_F(9),    KEY_F(21),    KEY_F(33),  KEY_F(45)},
 {SDLK_F10,  FALSE,  KEY_F(10),   KEY_F(22),    KEY_F(34),  KEY_F(46)},
 {SDLK_F11,  FALSE,  KEY_F(11),   KEY_F(23),    KEY_F(35),  KEY_F(47)},
 {SDLK_F12,  FALSE,  KEY_F(12),   KEY_F(24),    KEY_F(36),  KEY_F(48)},
 {SDLK_F13,  FALSE,  KEY_F(13),   KEY_F(25),    KEY_F(37),  KEY_F(49)},
 {SDLK_F14,  FALSE,  KEY_F(14),   KEY_F(26),    KEY_F(38),  KEY_F(50)},
 {SDLK_F15,  FALSE,  KEY_F(15),   KEY_F(27),    KEY_F(39),  KEY_F(51)},
 {SDLK_BACKSPACE,FALSE,  0x08,        0x08,     CTL_BKSP,  ALT_BKSP},
 {SDLK_TAB,  FALSE,  0x09,        KEY_BTAB,     CTL_TAB,  ALT_TAB},
 {SDLK_PAUSE,  FALSE,  KEY_SUSPEND, KEY_SSUSPEND, KEY_SUSPEND, KEY_SUSPEND},
 {SDLK_CLEAR,  FALSE,  KEY_CLEAR,   KEY_CLEAR,    KEY_CLEAR,  KEY_CLEAR},
 {SDLK_HELP,  FALSE,  KEY_HELP,    KEY_SHELP,    KEY_LHELP,  KEY_HELP},
 {SDLK_MENU,  FALSE,  KEY_OPTIONS, KEY_SOPTIONS, KEY_OPTIONS, KEY_OPTIONS},
 {SDLK_ESCAPE,  FALSE,  0x1B,        0x1B,     0x1B,  ALT_ESC},
 {SDLK_KP_ENTER,TRUE,  '\n',    '\n',     CTL_PADENTER,ALT_PADENTER},
 {SDLK_KP_PLUS,  TRUE,  PADPLUS,     '+',     CTL_PADPLUS, ALT_PADPLUS},
 {SDLK_KP_MINUS,TRUE,  PADMINUS,    '-',     CTL_PADMINUS,ALT_PADMINUS},
 {SDLK_KP_MULTIPLY,TRUE,PADSTAR,     '*',     CTL_PADSTAR, ALT_PADSTAR},
 {SDLK_KP_DIVIDE,TRUE,  PADSLASH,    '/',     CTL_PADSLASH,ALT_PADSLASH},
 {SDLK_KP_PERIOD,TRUE,  PADSTOP,     '.',     CTL_PADSTOP, ALT_PADSTOP},
 {SDLK_KP_0,  TRUE,  '0',       '0',     CTL_PAD0,  ALT_PAD0},
 {SDLK_KP_1,  TRUE,  '1',      '1',     CTL_PAD1,  ALT_PAD1},
 {SDLK_KP_2,  TRUE,  '2',      '2',     CTL_PAD2,  ALT_PAD2},
 {SDLK_KP_3,  TRUE,  '3',      '3',     CTL_PAD3,  ALT_PAD3},
 {SDLK_KP_4,  TRUE,  '4',      '4',     CTL_PAD4,  ALT_PAD4},
 {SDLK_KP_5,  TRUE,  '5',      '5',     CTL_PAD5,  ALT_PAD5},
 {SDLK_KP_6,  TRUE,  '6',      '6',     CTL_PAD6,  ALT_PAD6},
 {SDLK_KP_7,  TRUE,  '7',      '7',     CTL_PAD7,  ALT_PAD7},
 {SDLK_KP_8,  TRUE,  '8',      '8',     CTL_PAD8,  ALT_PAD8},
 {SDLK_KP_9,  TRUE,  '9',      '9',     CTL_PAD9,  ALT_PAD9},
 {SDLK_SPACE,   FALSE,  ' ', ' ', ' ', ' '},
 {SDLK_UNKNOWN,FALSE,0,0,0,0}
};
#endif

int processevent () { // Convert keys down to PDC key commands. Introduces priority if multiple keys are pressed.
#ifdef __NOSDL__
  //TODO: fix the nasty delay with standard curses
  int tmp = getch();
  flushinput();
  return tmp;
#else
  
  SDL_Event event;
  
  while ( blocking ? SDL_WaitEvent( &event ) : SDL_PollEvent( &event ) ) {

    switch (event.type) {
    case SDL_QUIT :
      exit(1);

    case SDL_KEYUP :
      return ERR;
    case SDL_KEYDOWN :
      if (event.key.repeat && !blocking) {
        continue;
      }
      for (int i = 0; key_table[i].keycode; i++) {
        if (key_table[i].keycode == event.key.keysym.sym) {
          return key_table[i].normal;
        }
      }

      return event.key.keysym.sym;
    }

  }
  
  // Manually handling these repeats with a delay of one frame.
  int key;
  const Uint8* state = SDL_GetKeyboardState(NULL);
#define PRESSED(k) state[SDL_GetScancodeFromKey(k)]
  if (PRESSED(SDLK_UP) || PRESSED(SDLK_w))
    key = KEY_UP;
  else if (PRESSED(SDLK_DOWN) || PRESSED(SDLK_s))
    key = KEY_DOWN;
  else if (PRESSED(SDLK_RIGHT) || PRESSED(SDLK_d))
    key = KEY_RIGHT;
  else if (PRESSED(SDLK_LEFT) || PRESSED(SDLK_a))
    key = KEY_LEFT;
  else
    key = ERR;
  if (key == oldkey) {
    return key;
  }
  oldkey = key;
  
  return ERR;
#endif //!NOSDL
}

int getinput() {
  int key;

  key = processevent();
  if (key == -1) key = processevent();
  return key;
}

void flushinput () {
  flushinp();
}

void restms(int mils) { // a sort of busy wait to catch keyboard input
  napms (mils);
}

int pollevent () {
#ifdef __NOSDL__
  return !(getch() == ERR);
#else
  return SDL_PollEvent( NULL );
#endif
}

void setblocking(bool _blocking) {
#ifndef __NOSDL__
  blocking = _blocking;
#else
  if (_blocking)
    nodelay(stdscr, 0); // blocking mode
  else
    nodelay(stdscr, 1); // non-blocking mode
#endif
}
