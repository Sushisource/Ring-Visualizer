#version 120
#extension GL_EXT_gpu_shader4 : enable

uniform vec3 lightpos;
uniform float radius;

vec4 thetaToRGB(float h, float s, float v)
{
	int i;
	i = int(floor(h));
	float f = h - i;
	if ( !bool(i&1) ) f = 1 - f; // if i is even
	float m = v * (1 - s);
	float n = v * (1 - s * f);
	switch (i) {
		case 6:
		case 0: return(vec4(v, n, m,v));
		case 1: return(vec4(n, v, m,v));
		case 2: return(vec4(m, v, n,v));
		case 3: return(vec4(m, n, v,v));
		case 4: return(vec4(n, m, v,v));
		case 5: return(vec4(v, m, n,v));
	} 
}

void main()
{
	vec2 a = gl_FragCoord.xy-lightpos.xy;
	vec2 b = vec2(0,180f);
	float radians = acos(dot(normalize(a),normalize(b)));
	//Although this 5 should be a 6, 5 gives us better coloring
	float theta = abs(radians)*(5/3.1415926f);
    float falloff = distance(gl_FragCoord.xy, lightpos.xy)-180;
    falloff = clamp(log(falloff/30),0.0,1.0);
	vec4 col = thetaToRGB(theta,1,1)*falloff;
    col.a = length(col.rgb); //Fade via saturation
	gl_FragColor = col;
}
