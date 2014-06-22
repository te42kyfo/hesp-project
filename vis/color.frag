#version 130

uniform sampler2D tex;

void main(void){

	vec3 normal  =  (texture2D(tex, gl_TexCoord[0].st ).xyz-0.5)*2.0;


	if( (normal.x*normal.x + normal.y*normal.y + normal.z*normal.z) < 0.00005f) {
		gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0);
		return;
	}



	normal = normalize(normal);

	//	normal.x *= -1;

	vec3 incoming = vec3( 0.0f, 0.0f, -1.0);

	vec3 light1 = normalize( vec3( 1.0, -1.0, 1.0) );
	vec3 light2 = vec3( 1.0, 0.0, 0.0);

	vec3 reflection = reflect( incoming, normal );

	float specular1 = dot( light1, reflection);
	float specular2 = dot( light2, reflection);
	float lambert1 = dot( light1, normal);
	float lambert2 = dot( light2, normal);

	specular1 = max( 0.0f, specular1 );
	lambert1= max( 0.0f, lambert1 );
	specular2 = max( 0.0f, specular2 );
	lambert2 = max( 0.0f, lambert2 );

	specular1 = pow(specular1, 60);
	specular2 = pow(specular2, 60);

	gl_FragColor =  vec4( vec3( 1.0, 0.0, 0.0) * (specular1+lambert1+0.05) +
						  vec3( 0.1, 0.7, 0.6) * (specular2+lambert2+0.05),
						  1.0f);
 		
}
