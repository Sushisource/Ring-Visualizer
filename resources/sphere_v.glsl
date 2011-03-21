#version 120
#define BIN_SIZE 213 //213 Is just what it is.
#define PI 3.14159265358979323846264

uniform float freqData[BIN_SIZE];
varying vec3 normal, lightDir[2], eyeVec;
varying vec4 ambientGlobal, ambient, colorMod;
varying float d[2];

/************** generic cartesian spherical conversion functions ***************/
struct spherical {
    float r, phi, theta;
};

spherical cartesianToSpherical(vec3 cPoint) {
    spherical sCoords;
    sCoords.r = length(cPoint);  
    sCoords.theta = acos(cPoint.z / sCoords.r);
    sCoords.phi = atan(cPoint.y,cPoint.x);
    return sCoords;
}

vec3 sphericalToCartesian(spherical sPoint) {
    vec3 cCoords;
    cCoords.x = sPoint.r * sin(sPoint.theta) * cos(sPoint.phi);
    cCoords.y = sPoint.r * sin(sPoint.theta) * sin(sPoint.phi);
    cCoords.z = sPoint.r * cos(sPoint.theta);
    return cCoords;
}
//------------------------------------------------
//Radial audio response
//takes a spherical coordinates as parameter
void audioDistort(inout spherical sPoint) {
    int fauxIndex = int(clamp((abs(sPoint.phi)/(2*PI))*BIN_SIZE,1,BIN_SIZE));
    float regionAvg=freqData[fauxIndex-1];
    float x = (log(float(fauxIndex)) / log(float(BIN_SIZE))) * BIN_SIZE;
    sPoint.r += clamp(200*(0.01+clamp(regionAvg*(x/BIN_SIZE)*log(float(BIN_SIZE-fauxIndex)), 0.0f, 2.0f)),0,100);
}

void main()
{	
    spherical sphereCoords = cartesianToSpherical(gl_Vertex.xyz);
    //Audio distortion
    audioDistort(sphereCoords);
    //More Nice shading
    colorMod = vec4(vec3(.1,.1,.1)*log(sphereCoords.r*4),1);
    //convert modified spherical coordindates back to cartesian
    vec3 outCoords = sphericalToCartesian(sphereCoords);
    gl_Position = gl_ModelViewProjectionMatrix* vec4(outCoords, 1.0);
    //Perform lighting calculations
    normal = normalize(gl_NormalMatrix * gl_Normal);
    vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
    lightDir[0] = vec3(gl_LightSource[0].position.xyz - vVertex);
    lightDir[1] = vec3(gl_LightSource[1].position.xyz - vVertex);
    eyeVec = -vVertex;
    /* The ambient terms have been separated since one of them */
    /* suffers attenuation. Note we only consider first light's ambience*/
    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;

    d[0] = length(lightDir[0]);
    d[1] = length(lightDir[1]);
}
