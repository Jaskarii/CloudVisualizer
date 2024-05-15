#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color; // Change location to 1
out vec3 fragColor; // Define a varying variable to pass color to fragment shader
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    fragColor = color; // Assign the color to fragColor
}

#shader fragment
#version 330 core

in vec3 fragColor; // Receive color from the vertex shader
out vec4 FragColor;

void main()
{
    FragColor = vec4(fragColor, 1.0); // Use the color passed from the vertex shader
}
