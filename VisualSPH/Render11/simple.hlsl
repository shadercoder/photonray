cbuffer everyFrame 
{
    row_major float4x4 gWVP;
};

struct VSIn
{
    float4 pos : POSITION;             
	float4 col : COLOR;
};

struct PSIn
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

struct PSOut
{
	float4 color : SV_Target;
};
//
// VS
//
PSIn VS(VSIn input)
{
    PSIn output = (PSIn)0;  
	output.pos = mul(input.pos, gWVP);			
    output.color = input.col;
    return output;
}

//
// PS 
//
PSOut PS(PSIn input)
{   
	PSOut output = (PSOut) 0;
	output.color = input.color;
    return output;	
}
