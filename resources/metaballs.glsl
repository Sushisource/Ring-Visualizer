//Metaball frag shader for ring
#version 120
#define NUM_BALLS 40
#extension GL_EXT_gpu_shader4 : enable

uniform vec3 lightpos; //Light position.
uniform vec3 balls[NUM_BALLS]; //Note, z is radius/magnitude

bool energyField(in vec2 p, in float squish, in float iso)
{
    float en = 0.0;
    bool result = false;
    for(int i=0; i<NUM_BALLS; ++i)
    {
        //For whatever reason these end up reflected over the circle... Flip it back.
        vec2 ball = reflect(balls[i].xy-lightpos.xy,vec2(0.0,1.0))+lightpos.xy;
        float radius = balls[i].z;
        float denom =  max(0.0001, pow(length(ball - p), squish));
        en += (radius / denom);
    }
    if(en > iso)
        result = true;
    return result;
}

void main()
{
    bool outside;
    /* gl_FragCoord.xy is in screen space / fragment coordinates */
    outside = energyField(gl_FragCoord.xy, 1.0, 50.0);
    //outside = pointShower(gl_FragCoord.xy);
    if(outside == true)
        gl_FragColor = vec4(0.8, 0.85, 0.9, 0.5);
    else
        discard;
}
