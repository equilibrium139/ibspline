#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <SDL_ttf.h>

class Button
{
public:
	Button(SDL_Rect rect, const char* text, SDL_Color text_color, TTF_Font* font, SDL_Renderer* renderer);
	void Draw(SDL_Renderer* renderer) const;
	bool PointInRect(SDL_Point p) const { return SDL_PointInRect(&p, &button_rect); }
	Button() = default;
private:
	SDL_Rect button_rect;
	SDL_Rect text_rect;
	SDL_Texture* texture;
};


#endif // !BUTTON_H

