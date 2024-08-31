#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "array.h"
#include "mesh.h"
#include "display.h"
#include "vector.h"

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { .x = 0, .y = 0, .z = 0 };
float fov_factor = 640;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    load_obj_file_data("./assets/mesh.obj");
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            }
            break;
    }
}

vec2_t project(vec3_t point) {
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z,
    };

    return projected_point;
}

void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; ++i) {
        face_t cube_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[cube_face.a - 1];
        face_vertices[1] = mesh.vertices[cube_face.b - 1];
        face_vertices[2] = mesh.vertices[cube_face.c - 1];

        triangle_t projected_triangle;

        vec3_t transformed_vertices[3];

        for (int j = 0; j < 3; ++j) {
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            transformed_vertex.z += 5;
            transformed_vertices[j] = transformed_vertex;
        }

        vec3_t ab = vec3_sub(transformed_vertices[1], transformed_vertices[0]);
        vec3_t ac = vec3_sub(transformed_vertices[2], transformed_vertices[0]);
        vec3_t normal = vec3_cross(ab, ac);

        vec3_t camera_delta = vec3_sub(camera_position, transformed_vertices[0]);

        if (vec3_dot(camera_delta, normal) <= 0.0) {
            continue;
        }

        for (int j = 0; j < 3; ++j) {
            vec2_t projected_point = project(transformed_vertices[j]);
            projected_point.x += window_width * 0.5;
            projected_point.y += window_height * 0.5;

            projected_triangle.points[j] = projected_point;
        }

        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void) {
    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; ++i) {
        triangle_t triangle = triangles_to_render[i];

        draw_filled_triangle(
            triangle.points[0].x,
            triangle.points[0].y,
            triangle.points[1].x,
            triangle.points[1].y,
            triangle.points[2].x,
            triangle.points[2].y,
            0xFFFFFF00
        );
        // draw_filled_triangle(300, 100, 50, 400, 500, 700, 0xFFFFFF00);
    }

    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}
void free_resources(void) {
    free(color_buffer);

    array_free(mesh.vertices);
    array_free(mesh.faces);
}
int main(void) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}
