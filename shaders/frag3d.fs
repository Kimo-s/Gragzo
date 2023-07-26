#version 460 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 fragnormal;
in vec3 camPos;

const vec3 lightPos = vec3(5.0, 0.0, 0.0);
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

    vec3 finalcolor = ambientColor +  specColor * specular * lightColor * lightPower / dist + diffuseColor * lambertian * lightColor * lightPower / dist; //+ specColor * specular * lightColor * lightPower / dist;

    // finalcolor = pow(finalcolor, vec3(1.0/screenGamma));

    FragColor = vec4(finalcolor, 1.0f);
}