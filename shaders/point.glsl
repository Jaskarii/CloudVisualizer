#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.4, 1.0, 1.0); // Set fragment color to white
}