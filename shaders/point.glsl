#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in float depth;
layout(location = 2) in float isGround;

out float o_depth;
out float o_isGround;
uniform mat4 MVP;

void main()
{
    o_depth = depth;
    o_isGround = isGround;
    gl_Position = MVP * vec4(position, 1.0);
}

#shader fragment
#version 330 core

in float o_depth;
in float o_isGround;
out vec4 FragColor;

void main()
{
    if(o_isGround == 0.0)
    {
        // Brown color for ground
        FragColor = vec4(0.55, 0.27, 0.07, 1.0); // Saddle Brown
    }
    else
    {
        // Calculate the cube root of the depth
        float scaledDepth = pow(o_depth, 1.0 / 3.0);
        
        // Normalize the scaled depth to [0, 1]
        float normalizedDepth = scaledDepth / 20.0; // Adjust the divisor as necessary to fit your depth range
        normalizedDepth = clamp(normalizedDepth, 0.0, 1.0); // Ensure it's within [0, 1]

        // Assign color based on the normalized depth
        vec3 color;
        if (normalizedDepth < 0.5)
        {
            // From blue to yellow
            float t = normalizedDepth / 0.5;
            color = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 0.0), t);
        }
        else
        {
            // From yellow to red
            float t = (normalizedDepth - 0.5) / 0.5;
            color = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), t);
        }

        FragColor = vec4(color, 1.0);
    }
}