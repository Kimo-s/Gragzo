#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoords;

out vec2 fragUV;
out vec3 fragnormal;
out vec3 camPos;
out mat4 viewMat;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec4 position = view * model * vec4(aPos, 1.0);
    camPos = vec3(position)/position.w;

    viewMat = view;

    fragUV = uvCoords;
    fragnormal = vec3(transpose(inverse(view * model)) * vec4(normal,0.0));
}