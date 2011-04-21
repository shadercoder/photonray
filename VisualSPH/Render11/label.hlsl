Texture2D text;
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
    float4 pos : POSITION;             	
    float width : WIDTH;
	float height : HEIGHT;
};

struct VSParticleDrawOut
{
    float4 pos : POSITION;
    float width : WIDTH;
	float height : HEIGHT;
};

struct PSSceneIn
{
    float4 pos : SV_Position;
    float2 tex : TEXTURE;
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
	output.pos = input.pos;			
	output.width = input.width;
	output.height = input.height;
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
    float4 position = g_positions[0] * input[0].width * 0.5f;
    position = mul(position, ginvView) + input[0].pos;		
    output.pos = mul( position, gWVP );     
	output.tex = g_texcoords[0];
    SpriteStream.Append(output);
    
	position = g_positions[1] * input[0].height * 0.5f;
    position = mul(position, ginvView) + input[0].pos;		
    output.pos = mul( position, gWVP );     
	output.tex = g_texcoords[1];
    SpriteStream.Append(output);
    
	position = g_positions[2] * input[0].height * 0.5f;
    position = mul(position, ginvView) + input[0].pos;		
    output.pos = mul( position, gWVP );     
	output.tex = g_texcoords[2];
    SpriteStream.Append(output);

	position = g_positions[3] * input[0].width * 0.5f;
    position = mul(position, ginvView) + input[0].pos;		
    output.pos = mul( position, gWVP );     
	output.tex = g_texcoords[3];
    SpriteStream.Append(output);

    SpriteStream.RestartStrip();
}

//
// PS for particles
//
float4 PS(PSSceneIn input) : SV_Target
{
	float4 col = text.Sample( mysampler, input.tex );
    return col;	
}
