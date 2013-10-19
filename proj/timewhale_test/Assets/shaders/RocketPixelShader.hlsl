Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 color : COLOR;
    float2 tex : TEXCOORD0;
};

float4 RocketPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;

	// Sample the pixel color
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	// Multiply by the color
	//input.color.w = textureColor.w;
	textureColor = saturate(textureColor * input.color);
	//textureColor = input.color;

    return textureColor;
}