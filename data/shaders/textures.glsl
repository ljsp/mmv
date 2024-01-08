#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
layout(location= 0) in vec3 position;
layout(location= 1) in vec2 texcoord;
layout(location= 2) in vec3 normal;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);       // centre de la fenetre

    v_position= position;
    v_texcoord= texcoord;
    v_normal= normal;
}
#endif

#ifdef FRAGMENT_SHADER
in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

uniform sampler2D texture0;

void main( )
{

    gl_FragColor = texture(texture0, v_texcoord);
}
#endif

