cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	//float2 texCoord : TEXCOORD0;
};

struct PS_INPUT 
{ 
	float4 pos : SV_POSITION; 
	float4 color : COLOR; 
	//float2 texCoord : TEXCOORD0;
}; 

PS_INPUT mainVS(VS_INPUT IN)//float3 position : POSITION)
{ 
	PS_INPUT output;
	output.pos = mul(projectionMatrix, mul(viewMatrix, mul(worldMatrix, float4(IN.position, 1.0))));
	//output.pos = float4(position, 1.0);
	output.color = IN.color; 
	//output.texCoord = IN.texCoord;
	
	return output;
}