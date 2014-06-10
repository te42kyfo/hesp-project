#version 130

uniform sampler2D tex;

void main(void){

	float texValue = texture2D(tex, gl_TexCoord[0].st ).r;




	texValue *= 0.001;


	gl_FragColor = vec4( texValue*4.0,
						 texValue*2.0-0.25,
						 texValue*1.0-0.375,
						 1.0);


}
