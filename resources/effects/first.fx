#define MAX_POINT_LIGHTS 16

#define LIGHT_FLAG_ENABLED 0x1

matrix world;
matrix worldInvertedTransposed;
matrix view;
matrix projection;

float3 cameraPosition;


struct pointLignt {
    float3 position;
    float specularShininess;
    float3 amblient;
    float constantAttenuation;
    float3 diffuse;
    float linearAttenuation;
    float3 specular;   
    float quadraticAttenuation;

    float3 pad0;
    int flags;
};
// struct directLight {
//     float3 direction;

//     float3 amblient;
//     float3 diffuse;
//     float3 specular;

//     float specularShininess;
// };

cbuffer pointLightsCB {
    pointLignt pointLights[MAX_POINT_LIGHTS];
}
//float3(12.0, 5.0, 12.0), float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), 1.0, 1.0, 0.09, 0.032, true
float3 pointLightCalc(pointLignt light, float3 normal, float3 fragPos) {
    float distance = length(light.position - fragPos);
    float3 lightDir = normalize(light.position - fragPos);
    float3 viewDir = (cameraPosition - fragPos);
    float3 halfwayDir = normalize(lightDir + viewDir);

    float3 ambient = light.amblient;
    float3 diffuse = light.diffuse * dot(normal, lightDir);
    float3 specular = light.specular * pow(max(dot(normal, halfwayDir), 0.0), light.specularShininess);

    float attenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * distance + light.quadraticAttenuation * (distance * distance));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

// float3 directLightCalc(directLight light, float3 normal, float3 fragPos) {
//     float3 lightDir = normalize(-light.direction);
//     float3 viewDir = normalize(cameraPosition - fragPos);
//     float3 reflectDir = reflect(-lightDir, normal);

//     float3 ambient = light.amblient;
//     float3 diffuse = light.diffuse * max(dot(normal, lightDir), 0.0);
//     float3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), light.specularShininess);

//     return ambient + diffuse + specular;
// }

struct VS_INPUT {
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float4 color    : COLOR;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float3 fragPos  : TEXCOORD0;
    float3 normal   : TEXCOORD1;
    float3 normal2   : TEXCOORD2;
    float4 color    : COLOR;
};

PS_INPUT VSMain(VS_INPUT input) {
    PS_INPUT output;

    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    output.fragPos = mul(input.position, world).xyz;
    output.normal = mul(input.normal, (float3x3)worldInvertedTransposed);
    output.normal2 = input.normal;

    output.color = input.color;

    return output;
}

float4 PSMain(PS_INPUT input) : SV_Target {
    float3 normal = normalize(input.normal);

    float3 lightingColor = float3(0.0, 0.0, 0.0);

    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        if (pointLights[i].flags & LIGHT_FLAG_ENABLED)
            lightingColor += pointLightCalc(pointLights[i], normal, input.fragPos);
    }

    float3 resultColor = lightingColor * input.color.xyz;
    return float4(resultColor, 1.0);
}

technique10 MainRender {
    pass P0 {
        SetVertexShader(CompileShader(vs_4_0, VSMain()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PSMain()));
    }
}
