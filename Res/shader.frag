#version 330 core
out vec4 FragColor;

uniform vec4 drawColor;
uniform sampler2D inTexture;
uniform bool drawTexture;

in vec2 TexCoord;

void main()
{
    if(drawTexture){
        FragColor = texture(inTexture, TexCoord);
    }else{
        FragColor = drawColor;
    }

} 