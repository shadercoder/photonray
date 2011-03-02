Texture2D frontS;
Texture2D backS;
Texture3D volume;
Texture2D noise;
SamplerState mysampler;

cbuffer everyFrame
{
    row_major float4x4 mWorldViewProj;		
	float4 vLightPos1;
	float4 vLightPos2;
	float4 vDiffuseMaterial;
	float4 eye;
};

struct VS_INPUT 
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
	float4 col	: COLOR;
};

struct PS_INPUT 
{
	float4 pos : SV_Position;
	float3 norm : NORMAL;
	float4 col : COLOR;
};

struct PS_OUTPUT
{
    float4 color : SV_Target;  // Pixel color
};


PS_INPUT SimpleVS(VS_INPUT input )
{   
	PS_INPUT output;
	output.pos = mul(float4(input.pos, 1), mWorldViewProj);
	output.col = input.col;
	output.norm = float3(0.0f, 0.0f, 0.0f); 
	return output;
}

PS_OUTPUT SimplePS( PS_INPUT input)
{

    PS_OUTPUT outputColor = (PS_OUTPUT) 0; 
	outputColor.color = input.col;    
    return outputColor;
}

struct VS_IN 
{
	float3 pos : POSITION;
	float2 textcoord : TEXCOORD0;
	float4 col: COLOR;
};
struct PS_IN 
{
	float4 pos : SV_Position;
	float2 textcoord : TEXCOORD0;
	float4 col : COLOR;
};

PS_IN QuadVS(VS_IN input)
{
	PS_IN output = (PS_IN) 0;
	output.pos = float4(input.pos, 0);
	output.textcoord = input.textcoord;
	//output.col = input.col;
	output.col = float4(0.0f, 0.125f, 0.3f, 1.0f);
	output.pos = mul(float4(input.pos, 1), mWorldViewProj);
	return output;
}

float4 RayCastPS(PS_IN input): SV_Target
{	
	const int Iterations = 64;
	const float Threshold = 0.45;
	float StepSize = 1.7 / Iterations;
	float2 texC = input.textcoord; 
    float3 front = frontS.Sample(mysampler, texC).xyz;
    float3 back = backS.Sample(mysampler, texC).xyz;
    float3 dir = normalize(back - front);
    if (dot(front, back) == 0)
	{
		discard;
	}
    	
    float4 dst = input.col + float4(0, 0.02, 0.1, 0);
	dst.a = 0;
  
	float value = 0;	 
    float3 Step = dir * StepSize;		
	float4 halfStepBack = float4(-Step * 0.5, 0);
	float3 light1 = normalize(vLightPos1);	
	float3 light2 = normalize(vLightPos2);	
	float4 pos = float4(front - Step * noise.Sample(mysampler, float2(((int) (front.x * 1000)) % 32 / 32.0, ((int) (front.y * 1000)) % 32 / 32.0)).r, 0); 

	float E, N, U;
	float k;
	float3 normal, color;		
    for(int i = 0; i < Iterations; ++i)
    {		
        value = volume.Sample(mysampler, pos).r;
		if (value > Threshold)
		{
			pos += halfStepBack;
			value = volume.Sample(mysampler, pos).r;			
			if (value > Threshold) k = 0.5;
			else k = -0.5;
			pos += halfStepBack * k;
			value = volume.Sample(mysampler, pos).r;			
			E = volume.Sample(mysampler, pos + float4(StepSize, 0, 0, 0)).r;
			N = volume.Sample(mysampler, pos + float4(0, StepSize, 0, 0)).r;
			U = volume.Sample(mysampler, pos + float4(0, 0, StepSize, 0)).r;
			normal = normalize(float3(E - value, N - value, U - value));
			color = saturate((max(0, dot(normal, light1)) + max(0, dot(normal, light2))) * vDiffuseMaterial * 0.5 + 0.5);			
			dst = float4(color, 1);     
			break;     
		} 
		//advance the current position
		pos.xyz += Step;     
		//break if the position is greater than <1, 1, 1>
		if(pos.x > 1.0f || pos.y > 1.0f || pos.z > 1.0f)
			return dst;
	}
    return dst;
}
