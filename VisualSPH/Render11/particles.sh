Texture2D particle;
SamplerState mysampler; 

cbuffer everyFrame 
{
    row_major float4x4 gWVP;
	row_major float4x4 ginvView;
};

cbuffer cbImmutable 
{
    float4 g_positions[4];
    float4 g_texcoords[4];
};

struct VSParticleIn
{
    float3 pos : POSITION;             
	float4 col : COLOR;
};

struct VSParticleDrawOut
{
    float4 pos : POSITION;
    float4 color : COLOR0;
    float radius : RADIUS;
};

struct PSSceneIn
{
    float4 pos : SV_Position;
    float2 tex : TEXTURE0;
    float4 color : COLOR0;
};

//
// Vertex shader for drawing the point-sprite particles
//
VSParticleDrawOut VS(VSParticleIn input)
{
    VSParticleDrawOut output = (VSParticleDrawOut)0;  
    //
    // Pass the point through
    //    
	output.pos = float4(input.pos, 1);			
    output.radius = 0.025;
    output.color = input.col;//float4(0,0.2,1,1);    
    return output;
}

//
// GS for rendering point sprite particles.  Takes a point and turns it into 2 tris.
//
[maxvertexcount(4)]
void GS(point VSParticleDrawOut input[1], inout TriangleStream<PSSceneIn> SpriteStream)
{
    PSSceneIn output;
    
    //
    // Emit two new triangles
    //
    for(int i = 0; i < 4; ++i)
    {
        float4 position = g_positions[i] * input[0].radius;
        position = mul(position, ginvView) + input[0].pos;		
        output.pos = mul( position, gWVP );     
		output.color = input[0].color;
        output.tex = g_texcoords[i];
        SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();
}

//
// PS for particles
//
float4 PS(PSSceneIn input) : SV_Target
{   
    return particle.Sample( mysampler, input.tex ) * input.color;	
}
