#version 330 core

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_FragPos;

out vec4 FragColor;

uniform sampler2D u_DiffuseTexture;
uniform vec3 u_Color;
uniform vec2 u_TextureScale;
uniform vec2 u_TextureOffset;

void main()
{
    // Apply texture tiling and offset
    vec2 texCoord = v_TexCoord * u_TextureScale + u_TextureOffset;
    
    // Sample diffuse texture
    vec4 texColor = texture(u_DiffuseTexture, texCoord);
    
    // Multiply by material color
    vec3 finalColor = texColor.rgb * u_Color;
    
    FragColor = vec4(finalColor, texColor.a);
}
