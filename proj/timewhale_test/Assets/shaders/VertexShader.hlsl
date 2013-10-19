cbuffer MatrixBuffer
{
    matrix projectionMatrix;
	float4 screenWHcameraXY;
	float scale;
};

struct VertexInputType
{
    float4 position : POSITION;
	float3 color : COLOR;
    float4 tex : TEXCOORD0;
	float4 xywh : TEXCOORD1;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 color : COLOR;
    float2 tex : TEXCOORD0;
};

PixelInputType SpriteVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	float4x4 viewMatrix = {	1.0f, 0.0f, 0.0f, 0.0f, 
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 1.0f
						};   


	input.position.x *= (input.xywh.z / 2) * scale;
	input.position.x += (input.xywh.x - screenWHcameraXY.x / 2 + input.xywh.z / 2 + screenWHcameraXY.z) * scale;
	input.position.y *= (input.xywh.w / 2) * scale;
	input.position.y += (-input.xywh.y + screenWHcameraXY.y / 2 - input.xywh.w / 2 - screenWHcameraXY.w) * scale;

    output.position = mul(input.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates
	if(input.position.z == 0.0f || input.position.z == 1.0f) {
		output.tex.y = input.tex.y;
	}
	else {
		output.tex.y = input.tex.w;
	}
	
	if(input.position.z == 0.0f || input.position.z == 2.0f) {
		output.tex.x = input.tex.x;
	}
	else {
		output.tex.x = input.tex.z;
	}
	
	// Store the color
	output.color = input.color;
    
    return output;
}