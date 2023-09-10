#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "sdl_events.hpp"

using namespace std;

enum MouseState
{

};
enum EditState
{

};

//=== text art class ===
class Cell
{
	public:
		char c;
		bool select = false;
		SDL_Rect rect = {0,0,10,10};

		Cell() { //=== default cell.
			c = ' ';
		}
		Cell(char character) { //=== input value cell.
			c = character;
		}
		~Cell() {}
};
class TextArt
{
	public:
		vector<string> texts;
		vector< vector<Cell> > cells;
		int rowSize = 0;
		int colSize = 0;
		int rowCur = 0;
		int colCur = 0;

		TextArt(int r, int c) {
			rowSize = r;
			colSize = c;
			string line;
			for(int i=0; i<rowSize; i++) {
				line = "";
				vector<Cell> cline;
				for(int j=0; j<colSize; j++) {
					line += "a";
					Cell charCell('a');
					cline.push_back(charCell);
				}
				texts.push_back(line);
				cells.push_back(cline);
				cline.clear();
			}
		}
		~TextArt() {}

		void updateTextArt(int r, int c, char character) {
			texts[r][c] = character;
			cells[r][c].c = character;
		}
};

class Project
{
	public:
		//=== project info
		string name = "untitled";
		TTF_Font *font;
		SDL_Color fontColor = {255,255,255,255};
		bool saved = false;
		//=== textart backups
		vector<TextArt> textarts; //현재는 1개만 쓴다...
		int state = 0;

		//=== ready-made renders
		SDL_Texture *texture;
		vector<SDL_Surface*> backSurfs;
		vector<SDL_Surface*> textSurfs;
		SDL_Surface *chosSurf; //=== chosen layer
		SDL_Surface *seleSurf; //=== temp select layer
		//=== cursor
		SDL_Surface *curSurf;

		//=== project create types.	
		Project(TTF_Font *f) { //=== default empty project create
			TextArt initArt(10,20);
			textarts.push_back(initArt);
			font = f;
			formBaseLayers();
		}
		Project(TTF_Font *f, int r, int c) { //=== user stated project create
			TextArt initArt(r,c);
			textarts.push_back(initArt);
			font = f;
			formBaseLayers();
		}
		// Project() {} //=== ready-in project create.
		~Project() {}

		//=== forming surf functions.
		void formBaseLayers() {
			//=== pick the textart
			TextArt art = textarts[state];
			//=== make and save each line's surf.
			SDL_Surface *tempSurf;
			for (int i=0; i<(int)art.texts.size(); i++) {
				//=== text surf
				tempSurf = TTF_RenderUTF8_Blended(font, art.texts[i].c_str(), fontColor);
				textSurfs.push_back(tempSurf);
				// SDL_FreeSurface(tempSurf);
				//=== background surf
				tempSurf = SDL_CreateRGBSurface(0, textSurfs[i]->w,textSurfs[i]->h, 32, 0,0,0,0);
				backSurfs.push_back(tempSurf);
				// SDL_FreeSurface(tempSurf);
			}
		}
		void updateRender(int r) {
			TextArt art = textarts[state];
			SDL_Surface *tempSurf = textSurfs[r];
			textSurfs[r] = TTF_RenderUTF8_Blended(font, art.texts[r].c_str(), fontColor);
			SDL_FreeSurface(tempSurf);
			tempSurf = backSurfs[r];
			backSurfs[r] = SDL_CreateRGBSurface(0, textSurfs[r]->w,textSurfs[r]->h, 32, 0,0,0,0);
			SDL_FreeSurface(tempSurf);
			// SDL_FreeSurface(textSurfs[r]);
			// formBaseLayers();
		}

		//=== backup managements. 나중에
		void addStatus() {}
		void backStatus() {}
		void frontStatus() {}
};

//=== element classes ===
class Mouse
{
	public:
		int x;
		int y;
		bool hover = true;
		bool press = false;
		bool click = false;

		SDL_Rect mScreen = {0,0,0,0}; //=== click-able screen area.

		SDL_Rect mRect = {0,0,0,0}; //=== click-able area.
		int downCoord[2] = {-1,-1};
		int upCoord[2] = {-1,-1};

		void mouseEvent(SDL_Event *e) {
			//=== init mouse status.
			SDL_GetMouseState(&x, &y);
			// click = false;
			//=== hover.
			if(x>mRect.x && y>mRect.y && x<mRect.x+mRect.w && y<mRect.y+mRect.h) {
				hover = true;
			} else {
				hover = false;
			}
			//=== on hover, decide press/click/drag.
			if(hover) {
				if(e->type == SDL_MOUSEBUTTONDOWN) {
					press = true;
					downCoord[0] = x;
					downCoord[1] = y;
				}
				if(e->type == SDL_MOUSEBUTTONUP) {
					click = true;
					press = false;
					upCoord[0] = x;
					upCoord[1] = y;
				}
			}
		}

		void onEvent(SDL_Event *e) {
			// 	SDL_GetMouseState(&x, &y);
			// 	click = false;
			// 	cout<<x<<" "<<y<<endl;
			// 	if(mScreen.x<x && mScreen.y<y && mScreen.x+mScreen.w>x && mScreen.y+mScreen.h>y) {
			// 		hover = true;
			// 		if(e->type == SDL_MOUSEBUTTONDOWN) {
			// 			press = true;
			// 			cout<<"press"<<press<<click<<endl;
			// 		}
			// 		if(e->type == SDL_MOUSEBUTTONUP) {
			// 			if(press == true) {
			// 				click = true;
			// 				cout<<"click"<<press<<click<<endl;
			// 			}
			// 			press = false;
			// 		}
			// 	}
		}
		void setupMScreen(SDL_Rect rect) {
			mScreen.x = rect.x;
			mScreen.y = rect.y;
			mScreen.w = rect.w;
			mScreen.h = rect.h;
			mRect.x = rect.x;
			mRect.y = rect.y;
			mRect.w = rect.w;
			mRect.h = rect.h;
		}
};

class Button
{
	public:
		SDL_Texture *texture;
		SDL_Surface *surf;
		SDL_Rect rect = {0,0,0,0}; //=== rect to render-copy texture.

		SDL_Rect margin = {0,0,0,0};
		SDL_Rect border = {0,0,0,0};
		SDL_Rect padding = {0,0,0,0};
		SDL_Rect content = {0,0,0,0};
		int margSize = 5;
		int bordSize = 3;
		int paddSize = 5;

		string text;
		SDL_Color fontColor = {255,255,255,0xFF};
		SDL_Color bordColor = {255,255,255,255};
		SDL_Color backColor = {0,0,0,0};

		Button() : text("") {}
		Button(string t) : text(t) {}

		//=== funcs for drawing button.
		void drawButton(SDL_Renderer *renderer, TTF_Font *font) {
			//=== color button margin, border, padding.
			surf = SDL_CreateRGBSurface(0, margin.w,margin.h, 32, 0,0,0,0);
			SDL_FillRect(surf, &margin, SDL_MapRGBA(surf->format,0,0,0,0));
			SDL_FillRect(surf, &border, SDL_MapRGBA(surf->format,bordColor.r,bordColor.g,bordColor.b,bordColor.a));
			SDL_FillRect(surf, &padding, SDL_MapRGBA(surf->format,backColor.r,backColor.g,backColor.b,backColor.a));
			//=== blit text.
			SDL_Surface *tempSurf = TTF_RenderUTF8_Blended(font, text.c_str(), fontColor);
			SDL_BlitScaled(tempSurf, NULL, surf, &content);
			SDL_FreeSurface(tempSurf);

			//=== into texture
			texture = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_RenderCopy(renderer, texture, NULL, &rect);
			//=== free
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(texture);
		}
		void onMouseEvent(Mouse m, void (*func)(bool)) {
			if(margSize == 0) {
				//=== if no margin
				if(m.x>rect.x && m.x<rect.x+rect.w && m.y>rect.y && m.y<rect.y+rect.h) {
					if(m.press == false) {
						backColor = {100,100,100,255};
						if(m.click) {
							if(func != NULL) {
								func(m.click);
							}
						}
					} else { //= if m.press == true
						backColor = {150,150,150,255};
					}
				} else {
					backColor = {0,0,0,255};
				}
			} else {
				//=== if there is margin
				if(m.x>rect.x+margSize && m.x<rect.x+margSize+border.w && m.y>rect.y+margSize && m.y<rect.y+margSize+border.h) {
					if(m.press == false) {
						backColor = {100,100,100,255};
						if(m.click) {
							if(func != NULL) {
								func(m.click);
							}
						}
					} else { //= if m.press == true
						backColor = {150,150,150,255};
					}
				} else {
					backColor = {0,0,0,255};
				}
			}
		}

		//=== funcs for changing/setup button properties.
		void setupButtonLocation(int x, int y, TTF_Font *font) {
			int w, h;
			TTF_SizeText(font, text.c_str(), &w, &h);
			//=== margin
			margin.w = w + 2*margSize + 2*bordSize + 2*paddSize;
			margin.h = h + 2*margSize + 2*bordSize + 2*paddSize;
			//=== border
			border.x = margSize;
			border.y = margSize;
			border.w = w + 2*bordSize + 2*paddSize;
			border.h = h + 2*bordSize + 2*paddSize;
			//=== padding
			padding.x = margSize + bordSize;
			padding.y = margSize + bordSize;
			padding.w = w + 2*paddSize;
			padding.h = h + 2*paddSize;
			//=== content
			content.x = margSize + bordSize + paddSize;
			content.y = margSize + bordSize + paddSize;
			content.w = w;
			content.h = h;
			//=== rect
			rect.x = x, rect.y = y;
			rect.w = margin.w, rect.h = margin.h;
		}
		void setupButtonLocation(int x, int y, int wfix, int hfix, TTF_Font *font) {
			// margSize = 0;
			//=== rect
			rect.x = x, rect.y = y;
			rect.w = wfix, rect.h = hfix;
			//=== margin
			margin.w = wfix;
			margin.h = hfix;
			//=== border
			border.x = margSize;
			border.y = margSize;
			border.w = wfix - 2*margSize;
			border.h = hfix - 2*margSize;
			//=== padding
			padding.x = margSize + bordSize;
			padding.y = margSize + bordSize;
			padding.w = wfix - 2*margSize - 2*bordSize;
			padding.h = hfix - 2*margSize - 2*bordSize;
			//=== content
			content.x = margSize + bordSize + paddSize;
			content.y = margSize + bordSize + paddSize;
			content.w = wfix - 2*margSize - 2*bordSize - 2*paddSize;
			content.h = hfix - 2*margSize - 2*bordSize - 2*paddSize;
		}
		void setupButtonText(string s) {
			text.clear();
			text = s;
		}
		void setupButtonMBP(int m, int b, int p) {
			margSize = m;
			bordSize = b;
			paddSize = p;
		}
		void setupButtonColor(SDL_Color c) { //int r, int g, int b, int a
			bordColor.r = c.r;
			bordColor.g = c.g;
			bordColor.b = c.b;
			bordColor.a = c.a;
		}
		~Button() {}
};

class ScrollBar
{
	public:
		SDL_Texture *texture;
		SDL_Surface *surf;
		SDL_Rect rect = {0,0,0,0}; //=== rect to put it.

		//=== scrollbar prop.
		int scrollSize = 30;
		bool scrollType; //=== vert:T, hori:F

		//=== scrollbar buttons.
		Button btn0{""};
		Button btn1;
		Button btn2;

		ScrollBar(string s) {
			if(s == "vert") {
				scrollType = true;
				btn1.setupButtonText("^");
				btn2.setupButtonText("v");
			} else if (s == "hori") {
				scrollType = false;
				btn1.setupButtonText("<");
				btn2.setupButtonText(">");
			}
		}

		//=== draw scrollbar func.
		void drawScrollBar(SDL_Renderer *renderer, TTF_Font *font, Mouse m) {
			//=== mouse event.
			btn1.onMouseEvent(m, NULL);
			btn2.onMouseEvent(m, NULL);
			btn0.onMouseEvent(m, NULL);
			//=== button display.
			btn1.drawButton(renderer, font);
			btn2.drawButton(renderer, font);
			btn0.drawButton(renderer, font);
		}
		void onMouseEvent(Mouse m) {}

		//=== funcs for scrollbar properties.
		void setupScrollBarLocation(int x, int y, int len, TTF_Font *font) {
			rect.x = x, rect.y = y;
			if(scrollType) { //=== vertical.
				rect.w = scrollSize, rect.h = len;
				btn1.setupButtonLocation(rect.x, rect.y, rect.w, rect.w, font);
				btn2.setupButtonLocation(rect.x, rect.y+rect.h-rect.w, rect.w, rect.w, font);
				btn0.setupButtonLocation(rect.x, rect.y+rect.w, rect.w, rect.w, font);
			} else { //=== horizontal.
				rect.w = len, rect.h = scrollSize;
				btn1.setupButtonLocation(rect.x, rect.y, rect.h, rect.h, font);
				btn2.setupButtonLocation(rect.x+rect.w-rect.h, rect.y, rect.h, rect.h, font);
				btn0.setupButtonLocation(rect.x+rect.h, rect.y, rect.h, rect.h, font);
			}
		}
		void setupScrollBarSize(int size) {}
		void setupScrollBarProp() {}
};

class PopUp
{
	public:
		SDL_Texture *texture;
		SDL_Surface *surf;
		SDL_Rect rect = {0,0,0,0};
};

//=== button, tab class ===
class BaseBtn
{
	public:
		SDL_Texture *texture;
		SDL_Surface *surf;
		SDL_Rect rect = {0,0,0,0};

		// SDL_Rect area = {0,0,0,0};
		SDL_Color backc = {0,0,0,0};
		bool select = false;
		bool active = false;

		void setupPos(SDL_Rect r, int x, int y) {
			rect.x = r.x + x;
			rect.y = r.y + y;
		}
};

class PlainBtn : public BaseBtn
{
	public:
		SDL_Color idlec = {150,150,150,255};
		SDL_Color hoverc = {200,200,200,255};
		SDL_Color pressc = {100,100,100,255};

		void draw(SDL_Renderer *renderer, Mouse m) {
			//=== setup stuffs
			mouseOn(m);
			//=== draw.
			surf = SDL_CreateRGBSurface(0, rect.w,rect.h, 32, 0,0,0,0);
			SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, backc.r,backc.g,backc.b,backc.a));
			texture = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_RenderCopy(renderer, texture, NULL, &rect);
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(texture);
		}
		void mouseOn(Mouse m) {
			if(m.hover) { //=== when mouse in click-able area.
				if(m.x>rect.x && m.x<rect.x+rect.w && m.y>rect.y && m.y<rect.y+rect.h) { //=== if mouse on button
					if(m.press) { //=== pressed
						backc = pressc;
					} else { //=== just hover
						backc = hoverc;
					}
				} else {
					backc = idlec;
				}
			} else {
				backc = idlec;
			}
		}
};

class TextBtn : public BaseBtn
{
	public:
		SDL_Color idlec = {0,0,0,255};
		SDL_Color hoverc = {150,150,150,255};
		SDL_Color pressc = {100,100,100,255};

		string text;
		TTF_Font *font;
		SDL_Color fontc = {255,255,255,0xFF};

		TextBtn() : text("") {}
		TextBtn(TTF_Font *f, string s) : text(s) {
			font = f;
			TTF_SizeText(f, s.c_str(), &rect.w, &rect.h);
		}

		void draw(SDL_Renderer *renderer, Mouse m) {
			//=== setup stuffs
			SDL_Surface *tempSurf;
			SDL_Rect tempRect = {0,0,rect.w,rect.h};
			mouseOn(m);
			//=== draw back.
			surf = SDL_CreateRGBSurface(0, rect.w,rect.h, 32, 0,0,0,0);
			SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, backc.r,backc.g,backc.b,backc.a));
			//=== draw text part.s
			tempSurf = TTF_RenderUTF8_Blended(font, text.c_str(), fontc);
			SDL_BlitSurface(tempSurf, NULL, surf, &tempRect);
			SDL_FreeSurface(tempSurf);
			//=== textureize.
			texture = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_RenderCopy(renderer, texture, NULL, &rect);
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(texture);
		}
		void mouseOn(Mouse m) {
			if(m.hover) { //=== when mouse in click-able area.
				if(m.x>rect.x && m.x<rect.x+rect.w && m.y>rect.y && m.y<rect.y+rect.h) { //=== if mouse on button
					if(m.press) { //=== pressed
						backc = pressc;
					} else { //=== just hover
						backc = hoverc;
					}
				} else {
					backc = idlec;
				}
			} else {
				backc = idlec;
			}
		}

		void setupPadding(int padding) {}
};

class BorderBtn : public BaseBtn
{
	public:
		SDL_Rect margin = {0,0,0,0};
		SDL_Rect border = {0,0,0,0};
		SDL_Rect padding = {0,0,0,0};
		SDL_Rect content = {0,0,0,0};
		SDL_Color borderc = {255,255,255,255};

		string text;
		TTF_Font *font;
		SDL_Color fontc = {255,255,255,0xFF};

		BorderBtn() : text("") {}
		BorderBtn(TTF_Font *f, string s) : text(s) {
			Construct(f, s);
		}
		void Construct(TTF_Font *f, string s) {
			text = s;
			font = f;
			TTF_SizeText(f, s.c_str(), &content.w, &content.h);
			setupButtonMBP(2,2,1);
		}

		void draw(SDL_Renderer *renderer, Mouse m) {
			//=== setup stuffs
			SDL_Surface *tempSurf;
			mouseOn(m);
			//=== draw back. margin, border, padding.
			surf = SDL_CreateRGBSurface(0, rect.w,rect.h, 32, 0,0,0,0);
			SDL_FillRect(surf, &margin, SDL_MapRGBA(surf->format, 0,0,0,0));
			SDL_FillRect(surf, &border, SDL_MapRGBA(surf->format, borderc.r,borderc.g,borderc.b,borderc.a));
			SDL_FillRect(surf, &padding, SDL_MapRGBA(surf->format, backc.r,backc.g,backc.b,backc.a));
			//=== draw text part.
			tempSurf = TTF_RenderUTF8_Blended(font, text.c_str(), fontc);
			SDL_BlitSurface(tempSurf, NULL, surf, &content);
			SDL_FreeSurface(tempSurf);
			//=== textureize.
			texture = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_RenderCopy(renderer, texture, NULL, &rect);
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(texture);
		}
		void mouseOn(Mouse m) {
			active = false;
			if (m.hover) { //=== interactable button.
				if (m.x>rect.x && m.x<rect.x+rect.w && m.y>rect.y && m.y<rect.y+rect.h) { //=== if cursor on area,
					if (m.click) { //=== [click in btn area]
						if (select) { //=== when its correct btn click, activate.
							active = true;
							select = false;
							onClick(); //
						} else { //=== if its incorrect btn click, do nothing.
							backc = {0,0,0,255};
						}
					} else if (m.press) { //=== [pressed 'state' in btn area]
						if (select) { //=== if pressed state within correct selected btn.
							backc = {100,100,100,255};
						} else { //=== if pressed on: not yet selected or incorrect btn.
							int x = m.downCoord[0];
							int y = m.downCoord[1];
							if (x>rect.x && x<rect.x+rect.w && y>rect.y && y<rect.y+rect.h) { 
								select = true;
								backc = {100,100,100,255};
							} else {
								backc = {0,0,0,255};
							}
						}
					} else { //=== [no press, no click] ...hover
						backc = {150,150,150,255};
					}
				} else { //=== if cursor out of area,
					backc = {0,0,0,255};
					if (m.click) { //=== if click was from out of this btn, re-set select.
						select = false;
					}
				}
			} else { //=== disabled button.
				backc = {0,0,0,255};
			}
			// if(m.hover) { //=== when button is in mouse's click-able area.
			// 	if(m.x>rect.x && m.x<rect.x+rect.w && m.y>rect.y && m.y<rect.y+rect.h) { //=== if mouse on button
			// 		if(m.press) { //=== pressed
			// 			backc = {100,100,100,255};
			// 		} else if (m.click) { //=== clicked -> activate func.

			// 		} else { //=== just hover
			// 			backc = {150,150,150,255};
			// 		}
			// 	} else {
			// 		backc = {0,0,0,255};
			// 	}
			// } else {
			// 	backc = {0,0,0,255};
			// }
		}
		void onClick() {
			cout<<"clicked "<<text<<endl;
		}

		void setupButtonMBP(int m, int b, int p) {
			//=== margin
			margin.w = content.w + 2*m + 2*b + 2*p;
			margin.h = content.h + 2*m + 2*b + 2*p;
			//=== border
			border.x = m;
			border.y = m;
			border.w = content.w + 2*b + 2*p;
			border.h = content.h + 2*b + 2*p;
			//=== padding
			padding.x = m + b;
			padding.y = m + b;
			padding.w = content.w + 2*p;
			padding.h = content.h + 2*p;
			//=== content
			content.x = m + b + p;
			content.y = m + b + p;
			content.w = content.w;
			content.h = content.h;
			//=== rect
			rect.w = margin.w, rect.h = margin.h;
		}
		void setupBorderColor(SDL_Color c) {
			//=== border color
			borderc = c;
		}
};

//=== box classes ===
class Box
{
	public:
		//=== box layout
		SDL_Texture *texture;
		SDL_Surface *surf;
		SDL_Rect rect = {0,0,0,0};
		SDL_Rect border = {0,0,0,0};
		SDL_Rect outer = {0,0,0,0};
		SDL_Rect inner = {0,0,0,0};
		int borderSize = 3;
		SDL_Color borderColor = {255,255,255,255};

		//=== content var
		TTF_Font *contentfont;

		//=== draw box outline using obj's data saved above.
		void drawBox(SDL_Renderer *renderer, Mouse m) {
			//=== set box surf
			surf = SDL_CreateRGBSurface(0, rect.w,rect.h, 32, 0,0,0,0);
			SDL_FillRect(surf, &border, SDL_MapRGBA(surf->format, borderColor.r,borderColor.g,borderColor.b,borderColor.a));
			SDL_FillRect(surf, &outer, SDL_MapRGBA(surf->format, 0,0,0,255));
			SDL_FillRect(surf, &inner, SDL_MapRGBA(surf->format, 50,50,50,255));
			//=== draw box texture
			texture = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_RenderCopy(renderer, texture, NULL, &rect);
			//=== free
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(texture);
		}
		//=== change box size-related variables
		void resizeBox(int x, int y, int w, int h) {
			rect.x = x, rect.y = y, rect.w = w, rect.h = h;
			border.x = 0, border.y = 0, border.w = w, border.h = h;
			//== outer
			outer.x = border.x + borderSize;
			outer.y = border.y + borderSize;
			outer.w = border.w - 2*borderSize;
			outer.h = border.h - 2*borderSize;
			//=== inner
			inner.x = border.x + 2*borderSize;
			inner.y = border.y + 2*borderSize;
			inner.w = border.w - 4*borderSize;
			inner.h = border.h - 4*borderSize;
		}
		//=== change box's color(border)
		void recolorBox(int r, int g, int b, int a) {
			borderColor.r = r;
			borderColor.g = g;
			borderColor.b = b;
			borderColor.a = a;
		}
		~Box() {}
};

class BigBox : public Box
{
	public:
		//=== box tabs
		// vector<string> tabs;
		vector<BorderBtn> tabs;
		int current = 0;
		int tabH = 22;
		int tabP = 2;
		TTF_Font *tabfont;

		//=== change box size-related variables
		void resizeBox(int x, int y, int w, int h) {
			rect.x = x, rect.y = y, rect.w = w, rect.h = h;
			border.x = 0, border.y = tabH, border.w = w, border.h = h-tabH;
			//== outer
			outer.x = border.x + borderSize;
			outer.y = border.y + borderSize;
			outer.w = border.w - 2*borderSize;
			outer.h = border.h - 2*borderSize;
			//=== inner
			inner.x = border.x + 2*borderSize;
			inner.y = border.y + 2*borderSize;
			inner.w = border.w - 4*borderSize;
			inner.h = border.h - 4*borderSize;
		}
		//=== give desired font size(in index)
		int refontTab() {
			return tabH - 2*tabP - borderSize - 10;
		}
		~BigBox() {}
};

class EditBox : public BigBox
{
	public:
		//=== integrate with 'tabs'. display works.
		vector<Project> works;

		//=== tabs pre-setup btns.
		BorderBtn tabListBtn;
		BorderBtn tabAddBtn;

		//=== editbox needs.

		//=== cursor var
		SDL_Surface *curSurf;
		SDL_Rect curRect = {0,0,0,0};
		SDL_Color curColor = {0,255,0,0xFF};

		SDL_Texture *editScreen;
		//=== font var
		int fontSize = 10;
		SDL_Color fontColor = {255,255,255,0xFF};

		EditBox() {}
		void Construct(TTF_Font *tf, TTF_Font *cf) {
			tabfont = tf;
			contentfont = cf;
			//=== tab setup
			tabListBtn.Construct(tf, "...");
			tabListBtn.setupButtonMBP(0, borderSize, tabP);
			tabListBtn.setupBorderColor(borderColor);
			tabAddBtn.Construct(tf, "+");
			tabAddBtn.setupButtonMBP(0, borderSize, tabP);
			tabAddBtn.setupBorderColor(borderColor);
			//=== init work
			Project initProj(cf);
			works.push_back(initProj);
			//=== init work's tab
			BorderBtn initBtn(tf, initProj.name);
			initBtn.setupButtonMBP(0, borderSize, tabP);
			initBtn.setupBorderColor(borderColor);
			tabs.push_back(initBtn);
		}
		void changeInContent(char character) {
			Project *proj = &works[current]; //=== current tab's project
			TextArt *art = &proj->textarts[proj->state]; //=== textart's state
			art->updateTextArt(art->rowCur, art->colCur, character);
			if(art->colCur+1 >= art->colSize) {
					if(art->rowCur+1 >= art->rowSize) {}
					else {
						art->rowCur += 1;
						art->colCur = 0;
					}
				} else {
					art->colCur += 1;
				}
			proj->updateRender(art->rowCur);
		}
		void drawContent(SDL_Renderer *renderer, Mouse m, bool timeFlag) {
			//=== setup vars
			Project proj = works[current]; //=== current tab's project
			TextArt art = proj.textarts[proj.state]; //=== textart's state
			SDL_Texture *tempText;
			SDL_Surface *tempSurf;
			SDL_Rect tempRect;
			//=== draw each line
			int posx = 0;
			int posy = 0;
			for (int i=0; i<(int)proj.backSurfs.size(); i++) {
				tempRect = {inner.x, inner.y+posy, proj.backSurfs[i]->w, proj.backSurfs[i]->h};
				//=== back
				tempText = SDL_CreateTextureFromSurface(renderer, proj.backSurfs[i]);
				SDL_RenderCopy(renderer, tempText, NULL, &tempRect);
				SDL_DestroyTexture(tempText);
				//=== selection
				//=== mouse cursor
				if(m.x>tempRect.x && m.y>tempRect.y && m.x<tempRect.x+tempRect.w && m.y<tempRect.y+tempRect.h) {
					cout<<"d";
				}
				//=== cursor
				if (timeFlag) {
					if (i == art.rowCur) {
						SDL_Surface *tempSurf = SDL_CreateRGBSurface(0, art.cells[i][0].rect.w,art.cells[i][0].rect.h, 32, 0,0,0,0);
						SDL_FillRect(tempSurf, NULL, SDL_MapRGBA(tempSurf->format,0,180,0,255));
						tempText = SDL_CreateTextureFromSurface(renderer, tempSurf);
						SDL_RenderCopy(renderer, tempText, NULL, &art.cells[i][0].rect);
						//=== free
						SDL_FreeSurface(tempSurf);
						SDL_DestroyTexture(tempText);
					}
				}
				//=== text
				tempText = SDL_CreateTextureFromSurface(renderer, proj.textSurfs[i]);
				SDL_RenderCopy(renderer, tempText, NULL, &tempRect);
				SDL_DestroyTexture(tempText);
				posy += proj.backSurfs[i]->h;
			}
		}
		void drawContent2(SDL_Renderer *renderer, TextArt textArt, TTF_Font *font, bool timeFlag, Mouse m) {
			// //=== text display
			// SDL_Surface *tempSurf;
			// // SDL_Rect tempRect = {0,0,0,0};
			// SDL_Surface *textSurf;
			// SDL_Rect textRect = {0,0,0,0};

			// //=== content surf.
			// tempSurf = SDL_CreateRGBSurface(0, inner.w,inner.h, 32, 0,0,0,0);
			// SDL_FillRect(tempSurf, NULL, SDL_MapRGBA(tempSurf->format,50,50,50,255));

			// //=== blit each line. according to mouse hover.
			// for(int i=0; i<textArt.rowSize; i++) {
			// 	//=== make the text first
			// 	textSurf = TTF_RenderUTF8_Blended(font, textArt.text[i].c_str(), fontColor);
			// 	textRect.y = i*fontSize, textRect.w = textSurf->w, textRect.h = textSurf->h;
			// 	//=== color text's background.
			// 	if(m.x-inner.x>textRect.x && m.y-inner.y>textRect.y && m.x-inner.x<textRect.x+textRect.w && m.y-inner.y<textRect.y+textRect.h) {
			// 		SDL_Rect charRect = {textRect.x+(textRect.w/textArt.colSize)*(int)((m.x-inner.x)/(textRect.w/textArt.colSize)), textRect.y, textRect.w/textArt.colSize, textRect.h};
			// 		SDL_FillRect(tempSurf, &textRect, SDL_MapRGBA(tempSurf->format,0,0,0,255));
			// 		SDL_FillRect(tempSurf, &charRect, SDL_MapRGBA(tempSurf->format,100,100,100,255));
			// 	}
			// 	else {
			// 		SDL_FillRect(tempSurf, &textRect, SDL_MapRGBA(tempSurf->format,0,0,0,255));
			// 	}
			// 	//=== blit,free text.
			// 	SDL_BlitSurface(textSurf, NULL, tempSurf, &textRect);
			// 	SDL_FreeSurface(textSurf);
			// }

			// //=== cursor display
			// int w, h;
			// TTF_SizeText(font, "_", &w, &h);
			// if (timeFlag) {
			// 	curSurf = TTF_RenderText_Blended(font, "_", curColor);
			// } else {
			// 	curSurf = TTF_RenderText_Blended(font, " ", curColor);
			// }
			// curRect.x = w*textArt.colCur;
			// curRect.y = h*textArt.rowCur;
			// SDL_BlitSurface(curSurf, NULL, tempSurf, &curRect);
			// SDL_FreeSurface(curSurf);

			// //=== edit screen's surf -> texture
			// editScreen = SDL_CreateTextureFromSurface(renderer, tempSurf);
			// SDL_RenderCopy(renderer, editScreen, NULL, &inner);
			// //=== free
			// SDL_FreeSurface(tempSurf);
			// SDL_DestroyTexture(editScreen);
		}
		void drawTab(SDL_Renderer *renderer, Mouse m) {
			int posx = 0;
			int posy = 0;
			//=== starting btn
			tabAddBtn.setupPos(rect, posx, posy);
			tabAddBtn.draw(renderer, m);
			posx += tabAddBtn.rect.w - borderSize;
			//=== btns of projects.
			for (int i=0; i<(int)tabs.size(); i++) {
				//=== pos,draw each tabs.
				tabs[i].setupPos(rect, posx, posy);
				if (i == current) {
					tabs[i].padding.h = tabH;
				}
				tabs[i].draw(renderer, m);
				posx += tabs[i].rect.w - borderSize;
				//=== if tab is active, change current.
				if (tabs[i].active) {
					tabs[current].padding.h -= borderSize;
					current = i;
				}
			}
			//=== end of tabs btn
			posx = rect.w - tabListBtn.rect.w;
			tabListBtn.setupPos(rect, posx, posy);
			tabListBtn.draw(renderer, m);
		}
		~EditBox() {}

		//=== changes in tabs.
		void changeTab() {}
		//=== change font (content)
		void refontBox(TTF_Font *f) {
			contentfont = f;
		}


		void onMouseEvent(Mouse m, TextArt *textArt, TTF_Font *font) {
			//=== click
			if(m.click) {
				int w, h;
				TTF_SizeText(font, "_", &w, &h);
				if(m.x > inner.x && m.y > inner.y && m.x < inner.x+w*textArt->colSize && m.y < inner.y+h*textArt->rowSize) {
					textArt->colCur = (m.x-inner.x) / w;
					textArt->rowCur = (m.y-inner.y) / h;
				}
			}
		}
};

class SideBox : public BigBox
{
	public:
		//=== buttons
		// Button saveBtn{"Save"};
		// Button newBtn{"New"};
		//=== tab 0 element
		BorderBtn saveBtn;
		BorderBtn newBtn;
		//=== tab 1 element
		BorderBtn modeBtn;

		SideBox() {}
		void Construct(TTF_Font *tf, TTF_Font *cf) {
			tabfont = tf;
			contentfont = cf;
			//=== tab setup
			BorderBtn fileBtn(tf, "File");
			fileBtn.setupButtonMBP(0, borderSize, tabP);
			fileBtn.setupBorderColor(borderColor);
			tabs.push_back(fileBtn);
			BorderBtn editBtn(tf, "Edit");
			editBtn.setupButtonMBP(0, borderSize, tabP);
			editBtn.setupBorderColor(borderColor);
			tabs.push_back(editBtn);
			//=== tab 0 file
			saveBtn.Construct(cf, "Save");
			newBtn.Construct(cf, "New");
			//=== tab 1 edit
			modeBtn.Construct(cf, "Mode");
		}
		void drawContent(SDL_Renderer *renderer, Mouse m) {
			if(current == 0) { //=== 'file'
				saveBtn.setupPos(rect, inner.x, inner.y);
				saveBtn.draw(renderer, m);
				newBtn.setupPos(rect, inner.x+saveBtn.rect.w, inner.y);
				newBtn.draw(renderer, m);
			} else { //=== 'edit'
				modeBtn.setupPos(rect, inner.x, inner.y);
				modeBtn.draw(renderer, m);
			}
			// //=== button setup
			// saveBtn.setupButtonLocation(rect.x+inner.x, rect.y+inner.y, contentfont);
			// newBtn.setupButtonLocation(rect.x+inner.x+saveBtn.margin.w, rect.y+inner.y, contentfont);
			// //===button mouse hover/click setup
			// saveBtn.onMouseEvent(m, &func_btn_save);
			// newBtn.onMouseEvent(m, &func_btn_new);
			// //===button display
			// saveBtn.drawButton(renderer, contentfont);
			// newBtn.drawButton(renderer, contentfont);
		}
		void drawTab(SDL_Renderer *renderer, Mouse m) {
			int posx = 0;
			int posy = 0;
			for (int i=0; i<(int)tabs.size(); i++) {
				//=== pos,draw each tabs.
				tabs[i].setupPos(rect, posx, posy);
				if (i == current) {
					tabs[i].padding.h = tabH;
				}
				tabs[i].draw(renderer, m);
				posx += tabs[i].rect.w - borderSize;
				//=== if tab is active, change current.
				if (tabs[i].active) {
					tabs[current].padding.h -= borderSize;
					current = i;
				}
			}
		}
		~SideBox() {}
};

class DownBox : public BigBox
{
	public:
		DownBox() {}
		void Construct(TTF_Font *tf, TTF_Font *cf) {
			tabfont = tf;
			contentfont = cf;
		}
		void drawContent(SDL_Renderer *renderer, Mouse m) {}
		void drawTab(SDL_Renderer *renderer, Mouse m) {}
		~DownBox() {}
};