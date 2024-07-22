#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

uint32_t window_width;
uint32_t window_height;

SDL_Window* window;
SDL_Renderer* renderer;

uint32_t* color_buffer;
SDL_Texture* color_buffer_texture;

bool initialize_window(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_grid(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_window(void);


#endif // DISPLAY_H
