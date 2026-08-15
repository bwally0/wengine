#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out mat3 v_TBN;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
    // Transform position to world space
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    
    // Transform normal to world space (use normal matrix to handle non-uniform scaling)
    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
    v_Normal = normalize(normalMatrix * a_Normal);
    
    // Pass through texture coordinates
    v_TexCoord = a_TexCoord;
    
    // Calculate TBN matrix for normal mapping
    vec3 T = normalize(normalMatrix * a_Tangent);
    vec3 N = v_Normal;
    // Re-orthogonalize T with respect to N (Gram-Schmidt process)
    T = normalize(T - dot(T, N) * N);
    // Calculate bitangent
    vec3 B = cross(N, T);
    v_TBN = mat3(T, B, N);
    
    // Final position
    gl_Position = u_MVP * vec4(a_Position, 1.0);
}
