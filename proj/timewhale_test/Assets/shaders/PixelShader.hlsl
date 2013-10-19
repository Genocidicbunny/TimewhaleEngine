Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 color : COLOR;
    float2 tex : TEXCOORD0;
};

float4 SpritePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	
	// Sample the pixel color
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	if(input.color.x == 0.0f && input.color.y == 0.0f && input.color.z == 0.0f) {
		return textureColor;
	}
	else {
		float3 teamColor = input.color;
		float3 teamInverse = float3(1.0f, 1.0f, 1.0f) - teamColor;
		
		float3 pixelColor = textureColor.r * teamColor + textureColor.b * teamInverse;
		
		return float4(pixelColor, textureColor.a);
	}

    return textureColor;
}