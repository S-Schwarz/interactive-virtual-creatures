#version 330 core
out vec4 FragColor;

uniform vec4 drawColor;
uniform sampler2D inTexture;
uniform bool drawTexture;
uniform bool transparent;

in vec2 TexCoord;

void main()
{
    if(drawTexture){
        if(transparent){
            FragColor = texture(inTexture, TexCoord) * vec4(1,1,1,0.1);
        }else{
            FragColor = texture(inTexture, TexCoord);
        }
    }else{
        FragColor = drawColor;
    }

} 