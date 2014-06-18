#version 130

varying vec4 world_space_coordinate;

uniform samplerCube tex;

void main(void){



	float r2 =
		gl_TexCoord[0].x*gl_TexCoord[0].x +
		gl_TexCoord[0].y*gl_TexCoord[0].y;


	vec3 normal;
	normal.xy = gl_TexCoord[0].xy;

	normal.z = sqrt( 1.0 - r2);

	vec3 light1 = normalize( vec3( 0.0, 0.0, 0.5) - world_space_coordinate.xyz);
	vec3 light2 = normalize( vec3( -2.0, 0.0, 0.0) - world_space_coordinate.xyz);

	vec3 incoming = normalize(  world_space_coordinate.xyz - vec3(0.0, 0.0, 3.0));
	vec3 reflection = reflect(incoming, normal);


	float specular1 = dot(reflection, light2);
	float specular2 = dot(reflection, light1);

	if( specular1 < 0) specular1 = 0;
	if( specular2 < 0) specular2 = 0;

	specular1 = pow( specular1, 15 );
	specular2 = pow( specular2, 15 );


	vec3 cube_color1 = textureCube(tex, reflection, 1.0 +r2).rgb;


	if( r2 < 1.0f )
		gl_FragColor =vec4( cube_color1 * 0.8 +
							vec3(0.3, 0.3, 0.3) * ( specular1*1.0) +
							vec3(0.3, 0.3, 0.3) * ( specular2*1.0), 8.0-r2*8.0);

	else
		gl_FragColor = vec4(0.0, 0.0, 1.0, 0.0);


}
