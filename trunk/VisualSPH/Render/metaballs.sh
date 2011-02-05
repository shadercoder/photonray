Texture2D frontS;
Texture2D backS;
Texture3D volume;
SamplerState mysampler;

cbuffer everyFrame
{
    row_major float4x4 mWorldViewProj;		
	float4 vLightPos;
	float4 vDiffuseMaterial;
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
	float StepSize = 1.7 / Iterations;
	float2 texC = input.textcoord; 
    float3 front = frontS.Sample(mysampler, texC).xyz;
    float3 back = backS.Sample(mysampler, texC).xyz;
    float3 dir = normalize(back - front);
    if (dot(front, back) == 0)
	{
		return input.col;
	}
    
	float4 pos = float4(front, 0); 
    float4 dst = input.col + float4(0, 0.02, 0.1, 0);
	dst.a = 0;
	float4 src = float4(0, 0, 0, 0);
	
	const float4 neighbors[6] = {
		float4(0, 0, -1, 0),
		float4(0, 0, 1, 0),
		float4(0, -1, 0, 0),
		float4(0, 1, 0, 0),
		float4(-1, 0, 0, 0),		
		float4(1, 0, 0, 0),			
		};
    
	float value = 0;
	 
    float3 Step = dir * StepSize;		
    for(int i = 0; i < Iterations; i++)
    {		
        value = volume.Sample(mysampler, pos).r;
		if (value > 0.5)
		{
			[unroll]
			for (int j = 0; j < 6; ++j)
			{
				value += 0.05f * volume.Sample(mysampler, pos + neighbors[j] * StepSize).r;
			}
			//src = (float4) value;
			//src.a = 1.0;
			float E = volume.Sample(mysampler, pos + float4(StepSize, 0, 0, 0)).r;
	        float N = volume.Sample(mysampler, pos + float4(0, StepSize, 0, 0)).r;
	        float U = volume.Sample(mysampler, pos + float4(0, 0, StepSize, 0)).r;
	        float3 normal = normalize(float3(E - value, N - value, U - value));
			float3 light = normalize(vLightPos);
			float3 color = saturate(max(0, dot(normal, light)) * vDiffuseMaterial);
			src = float4(color, 1);
			//src.a *= 0.5f; //reduce the alpha to have a more transparent result          
		    //Front to back blending
			//dst.rgb = dst.rgb + (1 - dst.a) * src.a * src.rgb;
			//dst.a   = dst.a   + (1 - dst.a) * src.a;  
			//src.rgb *= src.a;
			//dst = (1.0f - dst.a) * src + dst;     
			dst = float4(color, 1);
     
			//break from the loop when alpha gets high enough
			//if(dst.a >= 0.9f)
			break;     
		} 
		//advance the current position
		pos.xyz += Step;     
		//break if the position is greater than <1, 1, 1>
		if(pos.x > 1.0f || pos.y > 1.0f || pos.z > 1.0f)
			break;
	}
    return dst;
}
