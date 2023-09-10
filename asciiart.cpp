#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <windows.h>

#include "sdl_boxes.hpp"
// #include "sdl_events.hpp"
#include "win_filemanage.hpp"

#include "asciicalculate.hpp"

using namespace std;

int main(int argc, char **argv) 
{
	//=== sdl window & renderer
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window;
	SDL_Renderer *renderer;
	int winW = 700;
	int winH = 500;

	window = SDL_CreateWindow(
		"ASCII Art Editor",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        winW, winH,
        SDL_WINDOW_RESIZABLE
	);
	if(window == NULL) {
		cout << "Failed to create window\n";
		return -1;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);

	//=== sdl ttf
	TTF_Init();
	string fontPath = "assets/Ubuntu_Mono/UbuntuMono-Regular.ttf";
	vector<int> fontSizes = {10,11,12,13,14,15,16,17,18,19,20};
	vector<TTF_Font*> fonts;
	TTF_Font *font;
	for (int i=0; i<(int)fontSizes.size(); i++) {
		font = TTF_OpenFont(fontPath.c_str(), fontSizes[i]);
		fonts.push_back(font);
	}

	//=== sdl image
	IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);



	//=== set basic window's var, box, elements.
	int sideSize = 200;
	int downSize = 200;

	EditBox editBox;
	SideBox sideBox;
	DownBox downBox;
	editBox.resizeBox(0, 0, winW-sideSize, winH-downSize);
	sideBox.resizeBox(winW-sideSize, 0, sideSize, winH);
	downBox.resizeBox(0, winH-downSize, winW-sideSize, downSize);
	editBox.recolorBox(0,150,0,255);
	sideBox.recolorBox(150,0,0,255);
	downBox.recolorBox(0,0,150,255);

	// editBox.refontBox(fonts[2]);
	// sideBox.refontBox(fonts[2]);
	// downBox.refontBox(fonts[2]);
	editBox.Construct(fonts[sideBox.refontTab()], fonts[5]);
	sideBox.Construct(fonts[sideBox.refontTab()], fonts[5]);
	downBox.Construct(fonts[downBox.refontTab()], fonts[5]);

	//=== init: text art obj, cursor timing, mouse.
	TextArt textArt(10, 20);
	SDL_StartTextInput();

	time_t timeprv, timenow;
	float timecha;
	timeprv = time(NULL);
	bool timeFlag = true;

	Mouse mouse;
	SDL_Rect screenRect = {0,0,winW,winH};
	mouse.setupMScreen(screenRect);



	//=== main loop
	bool quit = false;
	SDL_Event e;
	while(!quit)
	{	
		//=== reset variables
		mouse.click = false;
		//=== event handles
		while(SDL_PollEvent(&e) != 0)
		{
			//=== on quit
			if(e.type == SDL_QUIT) {
				quit = true;
			}
			//=== on other keyboard events
			else if(e.type == SDL_KEYDOWN) {
				timeprv = time(NULL); //=== visible cursor during keydown 
				//=== ctrl + keys.
				if(SDL_GetModState() & KMOD_CTRL) {
					switch(e.key.keysym.sym) {
						case SDLK_s:
							func_write_file(&textArt.texts);
							break;
						case SDLK_z:
							cout<<"ctrlz"<<endl;
							break;
						case SDLK_o:
							func_win_open_dialog(&textArt.texts);
							textArt.rowSize = textArt.texts.size();
							textArt.colSize = textArt.texts[0].size();
							textArt.rowCur = 0;
							textArt.colCur = 0;
							break;
					}
				}
				//=== text-art edit related keys.
				else {
					switch(e.key.keysym.sym) {
						case SDLK_BACKSPACE:
							textArt.texts[textArt.rowCur][textArt.colCur] = ' ';
							break;
						case SDLK_RETURN:
							cout<<"line break"<<endl;
							break;
						case SDLK_UP:
							if (textArt.rowCur > 0) {
								textArt.rowCur -= 1;
							}
							break;
						case SDLK_DOWN:
							if (textArt.rowCur+1 < textArt.rowSize) {
								textArt.rowCur += 1;
							}
							break;
						case SDLK_LEFT:
							if (textArt.colCur > 0) {
								textArt.colCur -= 1;
							}
							break;
						case SDLK_RIGHT:
							if (textArt.colCur+1 < textArt.colSize) {
								textArt.colCur += 1;
							}
							break;
					}
				}
			}
			//=== on text inputs
			else if(e.type == SDL_TEXTINPUT) {
				textArt.texts[textArt.rowCur][textArt.colCur] = (e.text.text)[0];
				//temp if notepad mode. -> moved to editbox's changeInContent()
				// if(textArt.colCur+1 >= textArt.colSize) {
				// 	if(textArt.rowCur+1 >= textArt.rowSize) {}
				// 	else {
				// 		textArt.rowCur += 1;
				// 		textArt.colCur = 0;
				// 	}
				// } else {
				// 	textArt.colCur += 1;
				// }
				editBox.changeInContent((e.text.text)[0]);
			}
			//=== on window resize event
			else if(e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					SDL_GetWindowSize(window, &winW, &winH);
					//=== resize boxes
					editBox.resizeBox(0, 0, winW-sideSize, winH-downSize);
					sideBox.resizeBox(winW-sideSize, 0, sideSize, winH);
					downBox.resizeBox(0, winH-downSize, winW-sideSize, downSize);
					//=== resize mouse area.
					screenRect = {0,0,winW,winH};
					mouse.setupMScreen(screenRect);
				}
			}
			//=== on mouse events
			else if(e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
				//=== change mouse states onEvent.
				mouse.mouseEvent(&e);
			}
		}
		//=== clear & render texture
		SDL_RenderClear(renderer);

		//=== draw basic boxes in windows.
		editBox.drawBox(renderer, mouse);
		sideBox.drawBox(renderer, mouse);
		downBox.drawBox(renderer, mouse);

		//=== draw boxes's contents. (drawing in textures.)
		editBox.drawTab(renderer, mouse);
		editBox.drawContent(renderer, mouse, timeFlag);
		// editBox.onMouseEvent(mouse, &textArt, fonts[0]);
		// editBox.drawContent2(renderer, textArt, fonts[0], timeFlag, mouse);
		
		sideBox.drawTab(renderer, mouse);
		sideBox.drawContent(renderer, mouse);

		downBox.drawContent(renderer, mouse);
		downBox.drawTab(renderer, mouse);

		
		// PlainBtn plain;
		// plain.rect = {10,10,10,10};
		// plain.draw(renderer, mouse);

		//
		// TextBtn txtbtn(fonts[0], "txt btn");
		// txtbtn.draw(renderer, mouse);
		//
		// BorderBtn bordbtn(fonts[0], "txt btn");
		// bordbtn.rect.x = 10;
		// bordbtn.draw(renderer, mouse);

		//temp bar....
		// ScrollBar bar("vert");
		// bar.setupScrollBarLocation(winW-bar.scrollSize, 0, winH, font);
		// bar.drawScrollBar(renderer, font, mouse);

		//=== time count for cursor
		timenow = time(NULL);
		timecha = (float)(timenow-timeprv);
		if(timecha>=0 && timecha<1) {
			timeFlag = true;
		} else if(timecha>=1 && timecha<2) {
			timeFlag = false;
		} else {
			timeprv = time(NULL);
		}

		//=== render the texture
		SDL_RenderPresent(renderer);
	}

	//=== quit
	IMG_Quit();
	SDL_StopTextInput();
	for(int i=0; i<(int)fonts.size(); i++) {
		TTF_CloseFont(fonts[i]);
	}
	// TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}