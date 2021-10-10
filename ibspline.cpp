#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include "BSpline.h"
#include "Button.h"
#include "Point.h"

TTF_Font* sans;
SDL_Texture* axis_number_textures[10];
const int window_width = 1000;
const int window_height = 600;
const int max_window_x = window_width - 1;
const int max_window_y = window_height - 1;
const SDL_Rect graph_area = { 0, 0, 800, 600 };
const int max_graph_x = graph_area.w + graph_area.x - 1;
const int max_graph_y = graph_area.h + graph_area.y - 1;
int selected_control_point_index = -1;
BSpline bspline;
bool adding_new_control_point = false; // Indicates if the user has pressed the addcp_button
bool running = true;
Button addcp_button;
Button save_button;

SDL_Texture* TextureFromText(const char* text, SDL_Color color, SDL_Renderer* renderer);
void InitializeAxisNumberTextures(SDL_Renderer* renderer);
void PollEvents();
void DrawPoints(SDL_Renderer* renderer);
void DrawAxisIndicators(SDL_Renderer* renderer);
void MoveSelectedControlPoint(int x, int y);
void DrawBSpline(SDL_Renderer* renderer);
void DrawButtons(SDL_Renderer* renderer);

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: ibspline [num_points] [degree] or ibspline [filename]\n";
		return -1;
	}

	std::string argument = argv[1];
	if (std::isdigit(argument[0]))
	{
		int num_points = std::stoi(argument);
		if (argc < 3)
		{
			std::cout << "Usage: ibspline [num_points] [degree] or ibspline [filename]\n";
			return -1;
		}
		argument = argv[2];
		int degree = std::stoi(argument);
		if (degree >= num_points)
		{
			std::cout << "Degree must be less than number of points.\n";
			return -1;
		}
		bspline = BSpline(num_points, degree);
	}
	else
	{
		const char* filename = argument.c_str();
		bspline = BSpline(filename);
	}

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	SDL_Window* window = SDL_CreateWindow("ibezier", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	sans = TTF_OpenFont("OpenSans-Regular.ttf", 12);
	InitializeAxisNumberTextures(renderer);
	SDL_Color black = { 0, 0, 0};
	addcp_button = Button({ 800, 100, 50, 25 }, "Add CP", black, sans, renderer);
	save_button = Button({ 800, 175, 50, 25}, "Save", black, sans, renderer);

	while (running)
	{
		PollEvents();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		DrawPoints(renderer);
		DrawAxisIndicators(renderer);
		if (bspline.Complete())
		{
			DrawBSpline(renderer);
			DrawButtons(renderer);
		}
		SDL_RenderPresent(renderer);
	}

	TTF_Quit();
	SDL_Quit();

	return 0;
}

void InitializeAxisNumberTextures(SDL_Renderer* renderer)
{
	const char* axis_number_strings[10] =
	{
		"0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0"
	};
	SDL_Color white = { 255, 255, 255 };
	for (int i = 0; i < 10; i++) axis_number_textures[i] = TextureFromText(axis_number_strings[i], white, renderer);
}

SDL_Texture* TextureFromText(const char* text, SDL_Color color, SDL_Renderer* renderer)
{
	SDL_Surface* surface = TTF_RenderText_Solid(sans, text, color);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return message;
}

void SetSelectedControlPoint(int mouse_x, int mouse_y)
{
	for (int i = 0; i < bspline.ControlPoints().size(); i++)
	{
		ScreenPoint control_point_screen_location = GetScreenPoint(bspline.ControlPoints()[i]);
		if (std::abs(control_point_screen_location.x - mouse_x) < 10 &&
			std::abs(control_point_screen_location.y - mouse_y) < 10)
		{
			selected_control_point_index = i;
			return;
		}
	}

	// we can add a new point since bspline isn't complete
	if (!bspline.Complete())
	{
		bspline.AddControlPoint(Normalize(mouse_x, mouse_y));
		selected_control_point_index = bspline.ControlPoints().size() - 1;
	}
	else
	{
		selected_control_point_index = -1;
	}
}

void PollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			running = false;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			SDL_Point click_screen_location = { event.button.x, event.button.y };

			if (SDL_PointInRect(&click_screen_location, &graph_area))
			{
				SetSelectedControlPoint(event.button.x, event.button.y);

				if (adding_new_control_point && selected_control_point_index >= bspline.Degree() &&
					selected_control_point_index < bspline.ControlPoints().size() - 1)
				{
					bspline.AddControlPoint(selected_control_point_index);
					selected_control_point_index = -1;
					adding_new_control_point = false;
				}
			}
			else if (addcp_button.PointInRect(click_screen_location) && bspline.Complete())
			{
				adding_new_control_point = !adding_new_control_point;
			}
			else if (save_button.PointInRect(click_screen_location) && bspline.Complete())
			{
				bspline.Save("bspline.txt");
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
		{
			selected_control_point_index = -1;
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			if (selected_control_point_index >= 0 && !adding_new_control_point)
			{
				MoveSelectedControlPoint(event.motion.x, event.motion.y);
			}
		}
	}
}

void MoveSelectedControlPoint(int x, int y)
{
	Point point = Normalize(x, y);
	if (point.x < 0) point.x = 0;
	if (point.x > 1) point.x = 1;
	if (point.y < 0) point.x = 0;
	if (point.y > 1) point.x = 1;

	bspline.SetPoint(selected_control_point_index, point);
}

void DrawBSpline(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	std::vector<Point> polyline = bspline.GetPolyLine();
	for (int i = 0; i < polyline.size() - 1; i++)
	{
		ScreenPoint a = GetScreenPoint(polyline[i]);
		ScreenPoint b = GetScreenPoint(polyline[i + 1]);
		SDL_RenderDrawLine(renderer, a.x, a.y, b.x, b.y);
	}
}

void DrawButtons(SDL_Renderer* renderer)
{
	addcp_button.Draw(renderer);
	save_button.Draw(renderer);
}

void DrawPoints(SDL_Renderer* renderer)
{

	for (int i = 0; i < bspline.ControlPoints().size(); i++)
	{
		Point point = bspline.ControlPoints()[i];
		// Indicate which points can be selected for the control point insertion algorithm by coloring them
		// green
		if (adding_new_control_point && i >= bspline.Degree() && i < bspline.ControlPoints().size() - 1)
		{
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
		}
		ScreenPoint control_point_screen_location = GetScreenPoint(point);
		SDL_Rect point_rect;
		point_rect.x = control_point_screen_location.x - 2;
		point_rect.y = control_point_screen_location.y - 2;
		point_rect.w = 4;
		point_rect.h = 4;
		SDL_RenderDrawRect(renderer, &point_rect);
	}
}

void DrawAxisIndicators(SDL_Renderer* renderer)
{
	const int dx = graph_area.w / 10;
	const int dy = graph_area.h / 10;
	const float line_length = 5.0f;

	SDL_Color White = { 255, 255, 255 };

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	int texture_index = 8;
	// Y-axis
	for (int y = dy; y < max_graph_y; y += dy)
	{
		const int w = 20;
		const int h = 20;
		SDL_Rect rect;
		rect.x = line_length;
		rect.y = y - h / 2;
		rect.w = w;
		rect.h = h;

		SDL_RenderCopy(renderer, axis_number_textures[texture_index--], NULL, &rect);
		SDL_RenderDrawLine(renderer, 0, y, line_length, y);
	}

	// X-axis
	texture_index = 0;
	for (int x = dx; x <= graph_area.w; x += dx)
	{
		const int w = 20;
		const int h = 20;
		SDL_Rect rect;
		rect.x = x - w / 2;
		rect.y = max_graph_y - line_length * 4;
		rect.w = w;
		rect.h = h;

		SDL_RenderCopy(renderer, axis_number_textures[texture_index++], NULL, &rect);
		SDL_RenderDrawLine(renderer, x, max_graph_y, x, max_graph_y - line_length);
	}
}