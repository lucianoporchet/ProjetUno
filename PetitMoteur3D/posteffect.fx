struct VS_Sortie
{
	float4 Pos : SV_POSITION;
	float2 CoordTex : TEXCOORD0;
};

Texture2D textureEntree;    // la texture
SamplerState SampleState;   // l'etat de sampling

float distance;

//-------------------------------------------------------
// Vertex Shader "Nul"
//-------------------------------------------------------
VS_Sortie NulVS(float4 Pos : POSITION, float2 CoordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = Pos;
	sortie.CoordTex = CoordTex;

	return sortie;
}

//-----------------------------------------------------
// Pixel Shader "Nul"
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
	   /*couleur = couleur * 0.9 + ct * 0.1;*/ // Vous pouvez ?jouer? avec les %
	   couleur = couleur * 0.3 + ct * 0.7; // Vous pouvez ?jouer? avec les %

	   x = x + dx;
	   y = y + dy;

	   tc.x = (x + 1.0) / 2.0;
	   tc.y = (y + 1.0) / 2.0;
	}

	return couleur;
}


float4 TestPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;

	// Niveaux de gris/Noir et blanc
	ct = textureEntree.Sample(SampleState, tc);
	couleur = (ct.r + ct.y + ct.z) / 3.0f;
	//Noir et blanc
	if (couleur.x < 0.2 || couleur.x >0.9) 
	{
		couleur.x = 0;
	}
	else {
		couleur.x = 1.0f;
	}
	if (couleur.y < 0.2 || couleur.y >0.9)
	{
		couleur.y = 0;
	}
	else {
		couleur.y = 1.0f;
	}
	if (couleur.z < 0.2 || couleur.z >0.9)
	{
		couleur.z = 0;
	}
	else {
		couleur.z = 1.0f;
	}

	//// DeepFried/Sepia
	//couleur = 0;
	//couleur -= textureEntree.Sample(SampleState, tc - 0.003)*2.7f;
	//couleur += textureEntree.Sample(SampleState, tc + 0.003)*2.7f;
	//Sepia
	//couleur = (couleur.x + couleur.y + couleur.z) / 3.0f;


	//// Retro
	//tc.y = tc.y + (sin(tc.y * 100) * 0.03);
	//couleur = 0;
	//ct = textureEntree.Sample(SampleState, tc);
	//couleur = ct;

	//// Arc en ciel
	//couleur.x = ct.x*sin(tc.x*100)*2;
	//couleur.y = ct.y*cos(tc.x*150)*2;
	//couleur.z = ct.z*sin(tc.x*50)*2;




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

technique11 test
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 TestPS();
		SetGeometryShader(NULL);
	}
}
