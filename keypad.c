/*
 *  Application note: Simple Keylock / Keypad for AZ-Touch Pi0 (Raspberry Pi Zero)  
 *  Version 1.0
 *  Copyright (C) 2020  Hartmut Wendt  www.zihatec.de
 *  
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/   



#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <bcm2835.h>
#include <unistd.h>

#include "ili9340.h"
#include "xpt2046.h"

#define BeepPin 19
#define T_CS BCM2835_SPI_CS1
#define _DEBUG_ 0

TouchInfo tinfo;

unsigned char symbol[4][4] = {
  { '7', '8', '9' },
  { '4', '5', '6' },
  { '1', '2', '3' },
  { 'C', '0', ' ' }
};


int X,Y;
unsigned long Number = 0;
unsigned long codenum = 1234;	// define here a valid code
FontxFile fx32G[2];


int ReadTFTConfig(char *path, int *width, int *height, int *offsetx, int *offsety) {
  FILE *fp;
  char buff[128];

//  printf("path=%s\n",path);
  fp = fopen(path,"r");
  if(fp == NULL) return 0;
  while (fgets(buff,128,fp) != NULL) {
//    printf("buf=%s\n",buff);
//    printf("buff[0]=%x\n",buff[0]);
    if (buff[0] == '#') continue;
    if (buff[0] == 0x0a) continue;
    if (strncmp(buff, "width=", 6) == 0) {
      sscanf(buff, "width=%d height=%d",width,height);
if(_DEBUG_)printf("width=%d height=%d\n",*width,*height);
    } else if (strncmp(buff, "offsetx=", 8) == 0) {
      sscanf(buff, "offsetx=%d",offsetx);
if(_DEBUG_)printf("offsetx=%d\n",*offsetx);
    } else if (strncmp(buff, "offsety=", 8) == 0) {
      sscanf(buff, "offsety=%d",offsety);
if(_DEBUG_)printf("offsety=%d\n",*offsety);
    }
  }
  fclose(fp);
  return 1;
}


/********************************************************************//**
 * @brief     draws the keypad
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_BoxNButtons(void)
{
   uint8_t utf[64];
   //clear screen black
  lcdFillScreen(BLACK);
  
  //Draw the Result Box
  lcdDrawFillRect(0, 0, 240, 80, CYAN); 

  //Draw C and OK field   
  lcdDrawFillRect(160,260,240,320,RED);
  lcdDrawFillRect(0,260,80,320,GREEN);  

  //Draw Horizontal Lines
  for (int h=80; h<=320; h+=60)
  lcdDrawLine(0, h, 240, h, WHITE);

  //Draw Vertical Lines
  for (int v=80; v<=240; v+=80)
  lcdDrawLine(v, 80, v, 320, WHITE);

  //Display keypad lables 
  lcdSetFontDirection(DIRECTION180);	
  
  // draw keypad numbers  
  for (int j=0;j<4;j++) {
    for (int i=0;i<3;i++) {
	  utf[0] = symbol[j][i];
	  utf[1] = 0;
		
	  // draw numbers 		
      if ((j==3) && (i==2)) lcdDrawUTF8String(fx32G, 55, 310, "OK", BLACK); //OK button		
      else lcdDrawUTF8String(fx32G, 206 - (80*i), 130 + (60*j), utf, WHITE);
	  
	  // define touch screen fields
	  xptSetPoint(&tinfo, 240 - (80*i),160 + (60*j),160 - (80*i), 100 + (60*j), utf[0]);
      xptDump(&tinfo);
    }
  }

}

/********************************************************************//**
 * @brief     sound output via piezo beeper
 * @param[in] frequency in HZ
 * @param[in] duration in ms
 * @return    None
 *********************************************************************/
void buzzer(int frequency, int duration) {
  int freq = (1000000 / frequency) / 2; 
  if(_DEBUG_)printf("frequency=%d\n",freq);
  int count = (duration * 1000) / (freq * 2);
  if(_DEBUG_)printf("counts=%d\n",count);
  for (int j=0;j<count;j++) {
    bcm2835_gpio_write(BeepPin, HIGH);
	usleep(freq);
    bcm2835_gpio_write(BeepPin, LOW);
    usleep(freq);
  }	
}	


/********************************************************************//**
 * @brief     check the entered number and shows result
 * @param[in] None
 * @return    None
 *********************************************************************/
void DisplayResult(void)
{
  lcdDrawFillRect(0, 0, 240, 80, CYAN);	
  if (Number == codenum) {
   	lcdDrawUTF8String(fx32G, 200, 60, "CODE OK", BLACK);
   	buzzer(1000,500);
	sleep(2);
  } else {
   	lcdDrawUTF8String(fx32G, 200, 60, "WRONG CODE", BLACK);
	for (int i=0;i< 3;i++) {
		  buzzer(2000,100);
          delay(50);      
        }
	sleep(2);
  }
}





int main(int argc, char **argv)
{
  int i;
  int screenWidth = 0;
  int screenHeight = 0;
  int offsetx = 0;
  int offsety = 0;
  uint16_t xpos,ypos;
  uint16_t color;
  uint8_t utf[64];
  char dir[128];
  char cpath[128];

if(_DEBUG_)  printf("argv[0]=%s\n",argv[0]);
  strcpy(dir, argv[0]);
  for(i=strlen(dir);i>0;i--) {
    if (dir[i-1] == '/') {
      dir[i] = 0;
      break;
    } // end if
  } // end for
if(_DEBUG_)printf("dir=%s\n",dir);
  strcpy(cpath,dir);
  strcat(cpath,"tft.conf");
if(_DEBUG_)printf("cpath=%s\n",cpath);
  if (ReadTFTConfig(cpath, &screenWidth, &screenHeight, &offsetx, &offsety) == 0) {
    printf("%s Not found\n",cpath);
    return 0;
  }
if(_DEBUG_)printf("ReadTFTConfig:screenWidth=%d height=%d\n",screenWidth, screenHeight);
  printf("Your TFT resolution is %d x %d.\n",screenWidth, screenHeight);
  printf("Your TFT offsetx    is %d.\n",offsetx);
  printf("Your TFT offsety    is %d.\n",offsety);

#if 0
  if (bcm2835_init() == -1) {
    printf("bmc2835_init Error\n");
    return 1;
  }
#endif

/*
// Beeper Pin definitions
#ifdef BCM
  bcm2835_gpio_fsel(BeepPin,BCM2835_GPIO_FSEL_OUTP); // beeper pin GPIO 19
#endif
*/
  // define font
  Fontx_init(fx32G,"./fontx/ILGH32XB.FNT","./fontx/ILGZ32XB.FNT"); // 16x32Dot Gothic

  xptInit(&tinfo);

  lcdInit(screenWidth, screenHeight, offsetx, offsety);
  lcdReset();
  lcdSetup();
  draw_BoxNButtons();

  // read xpt2046
  int pen_irq;
  int id;

  bcm2835_gpio_fsel(RPI_V2_GPIO_P1_13,BCM2835_GPIO_FSEL_INPT);

  xpos = 230;
  ypos = 60;
  for (;;) {
    usleep(10000);      /* do it anyway ; settle time when pen goes up */
    pen_irq = bcm2835_gpio_lev(RPI_V2_GPIO_P1_13);
    if (pen_irq == LOW) { /* P1.18 == PenIRQ is LOW : touch! pen is down */
      id = xptGetPoint(T_CS, &tinfo);
      if (id != -1) {
		if(_DEBUG_)printf("button id=%d\n",id);
		lcdInit(screenWidth, screenHeight, offsetx, offsety);
        lcdSetFontDirection(DIRECTION180);
		// detect button 
		if ((id == ' ') || (id == 'C')) { // ok or clear
		  // ok button pressed?
		  if (id == ' ') DisplayResult();
		  
		  //Clear Result Box
		  xpos = 230;
		  Number = 0;
		  lcdDrawFillRect(0, 0, 240, 80, CYAN); 
		  
		} else if (Number < 999999) {					// number button
		  Number = (Number * 10) + (id - 48);	
		  if(_DEBUG_)printf("Entered number is: %ld\n",Number);
          xpos = lcdDrawUTF8String(fx32G, xpos, ypos, "*", BLACK);
		}  
      }
    }
  }

}




