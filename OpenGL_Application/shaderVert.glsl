#version 330 core

layout (location = 0) in vec3 Position;

// shader scale
//uniform float gScale;

// shader position
uniform mat4 gTranslation;

void main()
{
    //gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0);
    gl_Position = gTranslation * vec4(Position, 1.0); // order is important for dot product
}