#version 330 core

layout (location = 0) in vec3 Position;

// shader scale
//uniform float gScale;

// shader position
//uniform mat4 gTranslation;

// shader rotation
//uniform mat4 gRotation;

// shader scaling
uniform mat4 gScaling;

out vec4 Color;

// initialize arrays of vectors for Color RGBA
const vec4 colors[3] = vec4[3]( vec4(1, 0, 0, 1),
                                vec4(0, 1, 0, 1),
                                vec4(0, 0, 1, 1));


void main()
{
    //gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0);
    //gl_Position = gTranslation * vec4(Position, 1.0); // order is important for dot product
    //gl_Position = gRotation * vec4(Position, 1.0);
    gl_Position = gScaling * vec4(Position, 1.0);
    Color = colors[gl_VertexID];
}