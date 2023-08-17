#version 460 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 fragnormal;
in vec3 camPos;
in mat4 viewMat;

vec3 lightPos = vec3(0.0, 0.0, 5.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightPower = 40.0;
const vec3 ambientColor = vec3(0.1, 0.0, 0.0);
const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float shininess = 20.0;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space

void main()
{
    vec3 normal = normalize(fragnormal);
    vec4 lightPos4 = viewMat*vec4(lightPos,1.0);
    lightPos = vec3(lightPos4)/lightPos4.w;
    vec3 lightDir = lightPos - camPos;
    float dist = length(lightDir);
    dist = dist * dist;
    lightDir = normalize(lightDir);

    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;

    if(lambertian > 0.0) {
        vec3 viewDir = normalize(-camPos);

        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);
    }

    vec3 finalcolor = ourColor +  specColor * specular * lightColor * lightPower / dist + diffuseColor * lambertian * lightColor * lightPower / dist; //+ specColor * specular * lightColor * lightPower / dist;

    finalcolor = pow(finalcolor, vec3(1.0/screenGamma));

    FragColor = vec4(finalcolor, 1.0f);
}