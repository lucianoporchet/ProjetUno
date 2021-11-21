struct VS_Sortie
{
	float4 Pos : SV_POSITION;
	float2 CoordTex : TEXCOORD0;
};

Texture2D textureEntree;    // la texture
SamplerState SampleState;   // l'etat de sampling

float distance;

//-------------------------------------------------------
// Vertex Shader ?Nul?
//-------------------------------------------------------
VS_Sortie NulVS(float4 Pos : POSITION, float2 CoordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = Pos;
	sortie.CoordTex = CoordTex;

	return sortie;
}

//-----------------------------------------------------
// Pixel Shader ?Nul?
//-----------------------------------------------------
float4 NulPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;

	couleur = textureEntree.Sample(SampleState, vs.CoordTex);

	return couleur;
}

float4 RadialBlurPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;
	float d, dx, dy;

	couleur = 0;
	float x = tc.x * 2 - 1.0;
	float y = tc.y * 2 - 1.0;
	dx = sqrt(x*x);  // Distance du centre
	dy = sqrt(y*y);  // Distance du centre

	dx = x * (distance*dx);  	// Le d?grad? (blur) est en fonction de la
	dy = y * (distance*dy);	 	// distance du centre et de la variable distance.

	x = x - (dx * 10);   // Vous pouvez jouer avec le nombre d'it?rations
	y = y - (dy * 10);

	tc.x = (x + 1.0) / 2.0;
	tc.y = (y + 1.0) / 2.0;

	for (int i = 0; i < 10; i++)   // Vous pouvez jouer avec le nombre d'it?rations
	{
	   ct = textureEntree.Sample(SampleState, tc);
	   couleur = couleur * 0.6 + ct * 0.4; // Vous pouvez ?jouer? avec les %

	   x = x + dx;
	   y = y + dy;

	   tc.x = (x + 1.0) / 2.0;
	   tc.y = (y + 1.0) / 2.0;
	}

	return couleur;
}

technique11 Nul
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 NulPS();
		SetGeometryShader(NULL);
	}
}

technique11 RadialBlur
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 RadialBlurPS();
		SetGeometryShader(NULL);
	}
}
