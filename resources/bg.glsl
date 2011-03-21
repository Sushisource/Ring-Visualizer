//Frag shader for background
#version 120
#define BIN_SIZE 213 //213 Is just what it is.
#extension GL_EXT_gpu_shader4 : enable
#define ONE 0.00390625 //Texel offsets
#define ONEHALF 0.001953125
//Frequency information
uniform float freqData[BIN_SIZE];
uniform float time; // Used for texture animation
//Simplex noise texture
uniform sampler2D permTexture;

/*
 * 2D simplex noise. Adapted from Stefan Gustavson's (stegu@itn.liu.se) implementation.
 * I've modified it by 
 */
float snoise(vec2 P) 
{
    // Skew and unskew factors are a bit hairy for 2D, so define them as constants
    // This is (sqrt(3.0)-1.0)/2.0
    #define F2 0.366025403784
    // This is (3.0-sqrt(3.0))/6.0
    #define G2 0.211324865405
    // Skew the (x,y) space to determine which cell of 2 simplices we're in
    float s = (P.x + P.y) * F2;   // Hairy factor for 2D skewing
    vec2 Pi = floor(P + s);
    float t = (Pi.x + Pi.y) * G2; // Hairy factor for unskewing
    vec2 P0 = Pi - t; // Unskew the cell origin back to (x,y) space
    Pi = Pi * ONE + ONEHALF; // Integer part, scaled and offset for texture lookup

    vec2 Pf0 = P - P0;  // The x,y distances from the cell origin

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Find out whether we are above or below the x=y diagonal to
    // determine which of the two triangles we're in.
    vec2 o1;
    if(Pf0.x > Pf0.y) o1 = vec2(1.0, 0.0);  // +x, +y traversal order
    else o1 = vec2(0.0, 1.0);               // +y, +x traversal order

    // Noise contribution from simplex origin
    vec2 grad0 = texture2D(permTexture, Pi).rg * 4.0 - 1.0;
    float t0 = 0.5 - dot(Pf0, Pf0);
    float n0;
    if (t0 < 0.0) n0 = 0.0;
    else {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad0, Pf0);
    }

    // Noise contribution from middle corner
    vec2 Pf1 = Pf0 - o1 + G2;
    vec2 grad1 = texture2D(permTexture, Pi + o1*ONE).rg * 4.0 - 1.0;
    float t1 = 0.5 - dot(Pf1, Pf1);
    float n1;
    if (t1 < 0.0) n1 = 0.0;
    else {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad1, Pf1);
    }

    // Noise contribution from last corner
    vec2 Pf2 = Pf0 - vec2(1.0-2.0*G2);
    vec2 grad2 = texture2D(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
    float t2 = 0.5 - dot(Pf2, Pf2);
    float n2;
    if(t2 < 0.0) n2 = 0.0;
    else {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad2, Pf2);
    }

    // Sum up and scale the result to cover the range [-1,1]
    return 70.0 * (n0 + n1 + n2);
}

void main()
{
    vec4 color = vec4(1,1,1,1)*snoise(gl_FragCoord.xy);//vec3(gl_FragCoord.xy, time));
    color.a = 1;
    gl_FragColor = color;
}
