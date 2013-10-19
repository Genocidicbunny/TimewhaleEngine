cbuffer MatrixBuffer
{
    matrix projectionMatrix;
	float2 screenWH;
	float2 translateXY;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 color : COLOR;
    float2 tex : TEXCOORD0;
};

PixelInputType RocketVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	float4x4 viewMatrix = {	1.0f, 0.0f, 0.0f, 0.0f, 
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 1.0f
						};

	input.position.x = input.position.x - screenWH.x / 2 + translateXY.x;
	input.position.y = input.position.y + screenWH.y / 2 - translateXY.y;

    output.position = mul(input.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates
	output.tex = input.tex;
	
	// Store color
	output.color = input.color;
    
    return output;
}