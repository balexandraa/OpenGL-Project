#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 textcoord;

out vec2 passTexture;
out vec3 fNormal;
out vec4 fragmentPosEyeSpace;  

out vec4 fragPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix; 
 
void main() {

    fragmentPosEyeSpace = view * model * vec4(vertexPosition, 1.0f); 

    // pozitia pt iluminare punctiforma
    fragPos = model * vec4(vertexPosition, 1.0f);

    // transmitem coord texturii
    passTexture = textcoord;

    fNormal = normalize(normalMatrix * vertexNormal);

    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
