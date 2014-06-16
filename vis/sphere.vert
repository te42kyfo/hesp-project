#version 130

varying vec4 world_space_coordinate;

void main(void)  {
	gl_TexCoord[0] = (gl_Vertex);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	world_space_coordinate  = gl_Position;
}
