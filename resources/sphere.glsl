#version 120

uniform float channelAvgs[2];
varying vec3 normal, lightDir[2], eyeVec;
varying vec4 ambientGlobal, ambient, colorMod;
varying float d[2];

void main (void)
{
	vec4 final_color = ambientGlobal * colorMod;
	vec3 N = normalize(normal);
    int i;
    for (i=0; i<2; i++)
    {
        vec3 L = normalize(lightDir[i]);
        
        float lambertTerm = max(0,dot(N,L));
        
        if(lambertTerm > 0.0)
        {
            float att= 1.0 / ( gl_LightSource[i].constantAttenuation + 
                (gl_LightSource[i].linearAttenuation*d[i]) + 
                (gl_LightSource[i].quadraticAttenuation*d[i]*d[i]) );
            att *= channelAvgs[i];

            final_color += (gl_LightSource[i].diffuse * 
            gl_FrontMaterial.diffuse * 
            lambertTerm + ambient)* att;// *snoise(gl_FragCoord.xyz)*10;	
            
            vec3 E = normalize(eyeVec);
            
            float specular = pow( max(dot(N, E), 0.0), 
            gl_FrontMaterial.shininess );
            
            final_color += gl_LightSource[i].specular * 
            gl_FrontMaterial.specular * specular * att;
        }
    }
    //This divisior is a poor immitation at AO but works okay for this case
	gl_FragColor = final_color/(gl_FragCoord.z*10-8);
}

