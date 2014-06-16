#version 130

varying vec4 world_space_coordinate;

void main(void){



	float r2 =
		gl_TexCoord[0].x*gl_TexCoord[0].x +
		gl_TexCoord[0].y*gl_TexCoord[0].y;


	vec3 normal;
	normal.xy = gl_TexCoord[0].xy;
	normal.z = sqrt( 1.0 - r2);

	vec3 light1 = normalize( vec3( 0.0, 0.0, 0.5) - world_space_coordinate.xyz);
	vec3 light2 = normalize( vec3( -2.0, 0.0, 0.0) - world_space_coordinate.xyz);

	vec3 incoming = vec3(0.0, 0.0, -1.0);

	vec3 reflection = incoming - 2*dot(incoming, normal)*normal;



	float dot1 = dot(normal, light1);
	float dot2 = dot(normal, light2);
	float specular1 = dot(reflection, light2);
	float specular2 = dot(reflection, light1);

	if( dot1 < 0) dot1 = 0;
	if( dot2 < 0) dot2 = 0;
	if( specular1 < 0) specular1 = 0;
	if( specular2 < 0) specular2 = 0;

	specular1 = 1.0* pow( specular1, 10 );
	specular2 = 1.0* pow( specular2, 10 );



	if( r2 < 1.0f )
		gl_FragColor = vec4( vec3(1.0, 0.5, 0.5) * (specular1+dot1+0.1) +
							 vec3(0.0, 0.5, 1.0) * (specular2+dot2+0.1), 1.0);

	else
		gl_FragColor = vec4(0.0, 0.0, 1.0, 0.0);


}
