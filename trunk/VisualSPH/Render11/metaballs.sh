Texture2D frontS;
Texture2D backS;
Texture3D volume;
Texture2D noise;
Texture2D background;

SamplerState mysampler;

struct DirectionalLight
{
	float4 dir;
	float4 color;	
};

struct Material
{
	float Ka, Kd, Ks, A;
};

cbuffer everyFrame
{
    row_major float4x4 mWorldViewProj;		
};

cbuffer Immute
{
	float4 ambientLight;
	DirectionalLight l1;
	DirectionalLight l2;
	Material material;
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
	output.col = float4(0.552f, 0.713f, 0.803f, 1.0f);
	output.pos = mul(float4(input.pos, 1), mWorldViewProj);
	return output;
}

//--------------------------------------------------------------------------------------
// Phong Lighting Reflection Model
//--------------------------------------------------------------------------------------
float4 calcPhongLighting( Material M, float4 LColor, float3 N, float3 L, float3 V, float3 R )
{
	float4 Ia = M.Ka * ambientLight;
	float4 Id = M.Kd * saturate( dot(N,L) );
	float4 Is = M.Ks * pow( saturate(dot(R,V)), M.A );	
	return Ia + (Id + Is) * LColor;
}

//--------------------------------------------------------------------------------------
// Blinn-Phong Lighting Reflection Model
//--------------------------------------------------------------------------------------
float4 calcBlinnPhongLighting( Material M, float4 LColor, float3 N, float3 L, float3 H )
{
	float4 Ia = M.Ka * ambientLight;
	float4 Id = M.Kd * saturate( dot(N,L) * 0.5f + 0.5f );
	float4 Is = M.Ks * pow( saturate(dot(N,H)), M.A );
	return Ia + (Id + Is) * LColor;
}

float3 calcGradient(float3 pos, float value, float StepSize)
{
	float E = volume.SampleLevel(mysampler, pos + float3(StepSize, 0, 0), 0).r;
	float N = volume.SampleLevel(mysampler, pos + float3(0, StepSize, 0), 0).r;
	float U = volume.SampleLevel(mysampler, pos + float3(0, 0, StepSize), 0).r;
	float3 ret = normalize(float3(E - value, N - value, U - value));	
	return ret;
}

float3 calcWaterColor( Material M, float4 LColor, float3 N, float3 L, float3 V, float3 R )
{
	float4 Ia = M.Ka * ambientLight;
	float4 Id = M.Kd * saturate( dot(N,L) );
	//float4 Is = M.Ks * pow( saturate(dot(R,V)), M.A );	
	float3 reflected_eye_to_pixel_vector=-V+2*dot(V,N)*N;
	float3 pixel_to_light_vector = -L;
	// calculating fresnel factor 
	float r=(1.2-1.0)/(1.2+1.0);
	float fresnel_factor = max(0.0,min(1.0,r+(1.0-r)*pow(1.0-dot(N, -V),4)));
	float g_WaterSpecularPower = 1000.0f;
	float g_WaterSpecularIntensity = 350.f;
	float3 g_WaterSpecularColor= float3(1.0f, 1.0f, 1.0f);
	float specular_factor = fresnel_factor*pow(max(0, dot(pixel_to_light_vector, reflected_eye_to_pixel_vector)), g_WaterSpecularPower);
	
	return Ia + (Id + g_WaterSpecularIntensity * specular_factor * g_WaterSpecularColor * fresnel_factor) * LColor;
}


float4 RayCastPS(PS_IN input): SV_Target
{	
	const int Iterations = 768;
	const float Threshold = 50.0f;
	float StepSize = 1.7f / Iterations;
	float2 texC = input.textcoord; 
    float3 front = frontS.SampleLevel(mysampler, texC, 0).xyz;
    float3 back = backS.SampleLevel(mysampler, texC, 0).xyz;
    float3 dir = normalize(back - front);

	if (all(front == back))
	{
		discard;
	}

    float4 dst = float4( 0.552f, 0.713f, 0.803f, 1.0f );// = float4( 0.0f, 0.125f, 0.3f, 0.2f);
	dst.a = 0;
  
	float value = 0;	 
    float3 Step = dir * StepSize;		
	float3 halfStepBack = float3(-Step * 0.5);
	float3 light1 = normalize(l1.dir);	
	float3 light2 = normalize(l2.dir);	
	float3 pos = float3(front - Step * noise.SampleLevel(mysampler, float2(((uint) (front.x * (uint)1000)) % (uint)32 / 32.0, ((uint) (front.y * (uint)1000)) % (uint)32 / 32.0), 0).r); 
	
	//float E, N, U;
	float k;
	float3 normal, color;		
	float3 R;
    for(int i = 0; i < Iterations; ++i)
    {		
        value = volume.SampleLevel(mysampler, pos, 0).r;
		if (value > Threshold)
		{
			/*
			pos += halfStepBack;
			value = volume.SampleLevel(mysampler, pos, 0).r;			
			if (value > Threshold) k = 0.5;
			else k = -0.5;
			pos += halfStepBack * k;
			value = volume.SampleLevel(mysampler, pos, 0).r;			
			*/
			normal = calcGradient(pos, value, StepSize);
			R = reflect(light1, normal);						
			color = calcWaterColor( material, l1.color, normal, -light1, dir, R );// * float3(0.1,0.4,0.7);
			R = reflect(light2, normal);						
			color += calcWaterColor( material, l2.color, normal, -light2, dir, R );// * float3(0.1,0.4,0.7);
			color *= float3(0.1,0.4,0.7);
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
