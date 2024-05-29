#shader vertex
#version 330 core

layout(location = 0) in vec2 position;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

#shader geometry
#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * (gl_in[0].gl_Position + vec4(0.0, 0.0, -2.0, 0.0));
    EmitVertex();

    gl_Position = MVP * (gl_in[0].gl_Position + vec4(0.0, 0.0, 20.0, 0.0));
    EmitVertex();
}

#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.5, 1.0, 0.4); // Red color
}