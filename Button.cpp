#include "Button.h"

Button::Button(SDL_Rect rect, const char* text, SDL_Color text_color, TTF_Font* font, SDL_Renderer* renderer)
	:button_rect(rect)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, text_color);
	// Get the width and height needed to properly render text
	int w, h;
	TTF_SizeText(font, text, &w, &h);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	// Center text inside of button
	text_rect.x = button_rect.x + (button_rect.w - w) / 2;
	text_rect.y = button_rect.y + (button_rect.h - h) / 2;
	text_rect.w = w;
	text_rect.h = h;
}

void Button::Draw(SDL_Renderer* renderer) const
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &button_rect);
	SDL_RenderCopy(renderer, texture, NULL, &text_rect);
}
