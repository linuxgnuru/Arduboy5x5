//  Arduboy 5x5.
//  Port of the 5x5 puzzle game to the Arduboy platform.
//  Port of 5x5, 7x7 and 9x9 grids on both Cross and Knights format.
//  128 x 64 pixel screen prevents port over of 11x11 board (9x9 was a squeeze!)
//
//  John Stephenson  July 2017
//
#include <Arduboy2.h>
#include <ArduboyTones.h>
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
//      Program Memory
//      End Game Melody - Shave and a haircut, two bits.
const uint16_t tune[] PROGMEM = {NOTE_C5,256,NOTE_G4,96,NOTE_REST,32,NOTE_G4,128,
                                 NOTE_A4,256,NOTE_G4,256,NOTE_REST,256,NOTE_B4,256,
                                 NOTE_C5,256,NOTE_REST,2048,TONES_END};

//      Global Variables
byte    grid[81];   // Array of cells
byte    offset[14][2] = {{0,0},{0,-1},{-1,0},{1,0},{0,1},{0,0},{-1,-2},{1,-2},{-2,-1},{2,-1},{-2,1},{2,1},{-1,2},{1,2}};
byte    demo[176]      = {0,1,5,6,8,9,13,14,16,17,18,21,22,24,
                         0,1,3,5,6,7,8,9,11,12,13,15,16,18,19,21,27,29,30,32,33,35,36,37,39,40,41,42,43,45,47,48,
                         1,7,12,13,14,18,21,23,26,30,32,37,39,41,43,47,51,54,55,57,59,61,62,63,64,67,70,71,75,77,
                         0,2,4,6,7,8,10,11,13,14,16,17,18,20,22,24,
                         0,1,2,3,6,8,9,11,12,13,15,19,20,21,27,28,29,33,35,36,37,39,40,42,45,46,47,48,
                         0,1,4,7,8,9,10,11,12,13,14,15,16,17,18,19,22,25,26,27,29,31,33,35,
                         36,37,38,39,41,42,43,44,45,47,49,51,53,54,55,58,61,62,63,64,65,66,67,68,
                         69,70,71,72,73,76,79,80};
                         
char    menutext[7][8]={"Reset","5x5","7x7","9x9","Cross","Knights","Demo"};
byte    cells;      // Number of white Squares
byte    gridsize=5;
byte    cx, cy;
byte    ox, oy, ix, iy;
byte    o1=0,o2=8;
byte    i1=0,i2=4;
byte    menuoption=0;
bool    menuactive=false;
bool    demoactive=false;
bool    demoeven=false;   // True if an even number of passes have been made through the demo section.
byte    demoindex;
byte    knights=0;
bool    soundplayed=false;
byte    framerate=15,framecount=15;

//      Function prototypes
void    change_squares(byte x, byte y);
void    draw_square(byte x, byte y);
void    draw_menu();
void    draw_cursor(byte x, byte y);
void    puzzle_reset();
void    shuffle_demo(int strdemo, int enddemo);

void setup()
{
  gridsize = 5;
  cells = 0;
  arduboy.begin();
  arduboy.setFrameRate(framerate);
  sound.volumeMode(VOLUME_ALWAYS_HIGH);
  puzzle_reset();
}

void loop()
{
  // Wait for the start of the next frame
  if (!arduboy.nextFrame())
    return;
  // Maintain the frame count to act
  // as a 1 second timer for the demo mode.
  framecount--;
  if(!framecount)
     {
      framecount=framerate;
      if(demoactive)
      {
       demoeven=demoeven ? false : true;
       if(demoeven)
       {
        change_squares(demo[demoindex]%gridsize,demo[demoindex]/gridsize);
        demoindex++;
        if(cells==gridsize*gridsize)
          demoactive=false;
      }
       else
       {
        draw_cursor(demo[demoindex]%gridsize,demo[demoindex]/gridsize);
       }
       arduboy.display(); 
       return;
      }
     }
     
  // Bail out of loop() until the demo completes.
  // When the cell count = gridsize^2 in the above code, it will turn the demo mode off.
  if(demoactive)
    return;
     
  if(sound.playing())
  {
    // Keep the end of game display running until the tune ends.
    // This includes the two second silence at the end of the tune.
    // 'return' from the loop() function at the end of this block of
    // code to prevent any response until the tune has finished.
    
   soundplayed=true;
   change_squares(random(0,gridsize),random(0,gridsize));
   arduboy.display();
   return;
  }
  
  if(soundplayed)
  {
      soundplayed=false;
      demoactive=false;
      puzzle_reset();
  }
  
  if(cells==gridsize*gridsize)
    {
    if(!soundplayed)
     {
      sound.tones(tune);
      draw_square(cx, cy);
     }
    }
    
  arduboy.pollButtons();
  
  if (arduboy.justPressed(UP_BUTTON) == true)
  {
  if(menuactive)
  {
    if(menuoption>0)
    {
      menuoption--;
      draw_menu();
      arduboy.display();
    }
  }
  else
   if(cy > 0)
   {
    draw_square(cx, cy);
    cy--;
    draw_cursor(cx, cy);
    arduboy.display();
   }
  }
  
  if(arduboy.justPressed(DOWN_BUTTON) == true)
  if(menuactive)
  {
    if(menuoption<6)
    {
      menuoption++;
      draw_menu();
      arduboy.display();
    }
 }
 else   
  if(cy < gridsize-1)
  {
    draw_square(cx, cy);
    cy++;
    draw_cursor(cx, cy);
    arduboy.display();
  }
  
  if (arduboy.justPressed(LEFT_BUTTON) == true && cx > 0 && !menuactive)
  {
    draw_square(cx, cy);
    cx--;
    draw_cursor(cx, cy);
    arduboy.display();
  }
  
  if (arduboy.justPressed(RIGHT_BUTTON) == true && cx < gridsize-1 && !menuactive)
  {
    draw_square(cx, cy);
    cx++;
    draw_cursor(cx, cy);
    arduboy.display();
  }
  // If the menu is active, then execute the selected action............
  if (arduboy.justPressed(A_BUTTON) == true)
  {
    if(menuactive)
    {
      menuactive=false;
      switch(menuoption)
      {
        case 0:
          // Reset the current mode  
          puzzle_reset();
          break;
        case 1:
          // 5x5
          gridsize=5;
          puzzle_reset();
          break;
        case 2:
          // 7x7
          gridsize=7;
          puzzle_reset();
          break;
        case 3:
          // 9x9
          gridsize=9;
          puzzle_reset();
          break;
        case 4:
          // Cross pattern mode
          knights=0;
          puzzle_reset();
          break;
        case 5:
          // Knights pattern mode
          knights=1;
          puzzle_reset();
          break;
        case 6:
          // Start the demo mode
          demoactive=true;
          arduboy.initRandomSeed();
          framecount=framerate;
          // Set the demo offsets for Cross/Knights mode.
            switch(gridsize)
            {
             case 5:
               demoindex=knights ? 76:0;
               shuffle_demo(knights ? 76:0,knights ? 92:14);
               break;
             case 7:
               demoindex=knights ? 92:14;
               shuffle_demo(knights ? 92:14,knights ? 120:46);
               break;
             case 9:
               demoindex=knights ? 120:46;
               shuffle_demo(knights ? 120:46,knights ? 176:76);
               break;
            }
          demoeven=true;
          puzzle_reset();
          break;
       }
    }
    else
    // ...............  otherwise change the current square.
    {
     change_squares(cx, cy);
     draw_cursor(cx, cy);
     arduboy.display();
    }
  }
  
  // Toggle the menu active state
  if (arduboy.justPressed(B_BUTTON) == true)
  {
    menuactive=menuactive ? false:true;
    draw_menu();
    // Remove/replace the cursor square
    if(menuactive)
      draw_square(cx,cy);
    else
      draw_cursor(cx, cy);
    arduboy.display();
  }
}

// Draw the background grid pattern
void draw_grid()
{
  byte i;

  switch(gridsize)
  {
   case 5:
    for (i = 0; i < 6; i++)
    {
      arduboy.drawFastHLine(54, 2 + i * 12, 60, WHITE);
      arduboy.drawFastVLine(54 + i * 12, 2, 60, WHITE);
    }
    break;
    
    case 7:
     for (i = 0; i < 8; i++)
    {
      arduboy.drawFastHLine(54, 0 + i * 9, 64, WHITE);
      arduboy.drawFastVLine(54 + i * 9, 0, 64, WHITE);
    }
    break;

     case 9:
     for (i = 0; i < 10; i++)
    {
      arduboy.drawFastHLine(54, 4 + i * 6, 55, WHITE);
      arduboy.drawFastVLine(54 + i * 6, 4, 55, WHITE);
    }
    break;
  }
}

// Invert the squares within the board centred on x,y
// Cross pattern:   Use elements 0-4. Knights pattern: Use elements 5-13
void change_squares(byte x, byte y)
{
  byte nx, ny;
  byte i, first, last;
  
  first = knights * 5;
  last = knights * 9 + 5;

  for (i = first; i < last; i++)
    // Invert this square and those within range.
  {
    nx = x + offset[i][0];
    ny = y + offset[i][1];
    // If this square is within the current grid size......
    if (nx >= 0 && nx < gridsize && ny >= 0 && ny < gridsize)
    {
      // .......invert this square, and maintain the cell count.
      cells += (grid[ny * gridsize + nx] ^= 1) ? 1 : -1;

      // Draw this square
      draw_square(nx, ny);
    }
  }
}

// Draw the square at grid position  x , y
void draw_square(byte x, byte y)
{
  switch(gridsize)
  {
    case 5:
     arduboy.fillRect(56 + 12 * x, 4 + 12 * y, 9, 9, grid[y * gridsize + x] ? WHITE : BLACK);
     break;
     
    case 7:
     arduboy.fillRect(56 + 9 * x, 2 + 9 * y, 6, 6, grid[y * gridsize + x] ? WHITE : BLACK);
     break;
     
    case 9:
     arduboy.fillRect(55 + 6 * x, 5 + 6 * y, 5, 5, grid[y * gridsize + x] ? WHITE : BLACK);
     break;
  }
}

// Draw the cursor at grid position  x , y
void draw_cursor(byte x, byte y)
{
  switch(gridsize)
  {
  case 5:
     arduboy.fillRect(58 + 12 * x, 6 + 12 * y, 5, 5, grid[y * gridsize + x] ? BLACK : WHITE);
     break;
  case 7:
     arduboy.fillRect(57 + 9 * x, 3 + 9 * y, 4, 4, grid[y * gridsize + x] ? BLACK : WHITE);
     break;
  case 9:
     arduboy.fillRect(56 + 6 * x, 6 + 6 * y, 3, 3, grid[y * gridsize + x] ? BLACK : WHITE);
     break;
  }
}

// Draw the menu down the left side of the screen
void draw_menu()
{
  byte i;
  arduboy.setCursor(0,0);
  for(i=0;i<7;i++)
  {
    if(menuactive)
    {
     arduboy.setTextColor(i==menuoption ? BLACK:WHITE);
     arduboy.setTextBackground(i==menuoption ? WHITE:BLACK);
    }
    arduboy.println(menutext[i]);
  }
  // Leave set in white on black state.
  arduboy.setTextColor(WHITE);
  arduboy.setTextBackground(BLACK);
}

// Reset the puzzle
void puzzle_reset()
{
  arduboy.clear();
  memset(grid,0,sizeof grid);
  cells=0;
  draw_menu();
  cx = cy = gridsize/2;
  draw_grid();
  change_squares(cx, cy);
  if(!demoactive)
     draw_cursor(cx, cy);
  arduboy.display();
}

// Shuffle the moves between the start/end
// of the demo section for this mode.
void    shuffle_demo(int strdemo, int enddemo)
{
  int i,ix,iy;
  byte z;
   for(i=0;i<1000;i++)
   {
    ix=random(strdemo,enddemo);
    iy=random(strdemo,enddemo);
    // Swap these two elements
    z=demo[ix];
    demo[ix]=demo[iy];
    demo[iy]=z;
   }
}
