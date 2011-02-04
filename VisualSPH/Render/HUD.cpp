#include "DXUT.h"
#include "HUD.h"


HUD::HUD(void)
{
	text = L"";
}


HUD::~HUD(void)
{
	ReleaseCOM(mFont);
}

void HUD::init(ID3D10Device* device)
{
	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height          = 14;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
	fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    wcscpy(fontDesc.FaceName, L"Times New Roman");
	//strcpy(fontDesc.FaceName, "Arial");
	D3DX10CreateFontIndirect(device, &fontDesc, &mFont);
}

void HUD::draw()
{
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, text.c_str(), -1, &R, DT_NOCLIP, BLACK);

}

void HUD::setText(wstring& _text)
{
	text = _text;
}
