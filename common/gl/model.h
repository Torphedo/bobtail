#ifndef MODEL_H
#define MODEL_H
/// @file model.h
/// @brief Model loading utilities

#include "vector.h"

/// A vertex with only posiiton and color
typedef struct {
    vec3 position;
    vec4 color;
}vertex;

/// A vertex with position and texture coordinates
typedef struct {
    vec3 position;
    vec2 texcoord;
}tex_vertex;


/// Mesh data ready for rendering
/// TODO: Store vertex format size for easier uploading
typedef struct {
    const void* vertices;
    const u16* indices;
    u16 vert_count;
    u16 idx_count;
    gl_obj vao;
    gl_obj vbuf;
    gl_obj ibuf;
}model;

/// @brief Upload a model to the GPU
/// @param m Model to modify. OpenGL IDs for the GPU resources are written to
/// the model structure.
void model_upload(model* m);

/// Get the total memory footprint of a model in bytes
u32 model_size(const model m);

/// @brief Load OBJ data into a model struct.
///
/// Assumes vertex colors are stored as RGB values on each vertex.
model obj_load(u8* txt);

#endif // MODEL_H
