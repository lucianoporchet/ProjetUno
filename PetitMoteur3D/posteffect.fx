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
	   couleur = couleur * 0.5 + ct * 0.5; // Vous pouvez ?jouer? avec les %

	   x = x + dx;
	   y = y + dy;

	   tc.x = (x + 1.0) / 2.0;
	   tc.y = (y + 1.0) / 2.0;
	}

	return couleur;
}


float4 RetroPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;

	// Retro
	ct = textureEntree.Sample(SampleState, tc);
	couleur = (ct.r + ct.y + ct.z) / 3.0f;
	////Noir et blanc
	//if (couleur.x < 0.2 || couleur.x >0.9) 
	//{
	//	couleur.x = 0;
	//}
	//else {
	//	couleur.x = 1.0f;
	//}
	//if (couleur.y < 0.2 || couleur.y >0.9)
	//{
	//	couleur.y = 0;
	//}
	//else {
	//	couleur.y = 1.0f;
	//}
	//if (couleur.z < 0.2 || couleur.z >0.9)
	//{
	//	couleur.z = 0;
	//}
	//else {
	//	couleur.z = 1.0f;
	//}

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

float4 ProtanopePS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;

	ct = textureEntree.Sample(SampleState, tc);

	// RGB to LMS matrix conversion
	float3 L = (17.8824f * ct.x) + (43.5161f * ct.y) + (4.11935f * ct.z);
	float3 M = (3.45565f * ct.x) + (27.1554f * ct.y) + (3.86714f * ct.z);
	float3 S = (0.0299566f * ct.x) + (0.184309f * ct.y) + (1.46709f * ct.z);

	// Protanope - reds are greatly reduced
	float l = 0.0f * L + 2.02344f * M + -2.52581f * S;
	float m = 0.0f * L + 1.0f * M + 0.0f * S;
	float s = 0.0f * L + 0.0f * M + 1.0f * S;
	

	// LMS to RGB matrix conversion
	couleur.x = (0.0809444479f * l) + (-0.130504409f * m) + (0.116721066f * s);
	couleur.y = (-0.0102485335f * l) + (0.0540193266f * m) + (-0.113614708f * s);
	couleur.z = (-0.000365296938f * l) + (-0.00412161469f * m) + (0.693511405f * s);
	couleur.w = 1;

	return couleur;
}

float4 DeuteranopePS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;

	ct = textureEntree.Sample(SampleState, tc);

	// RGB to LMS matrix conversion
	float3 L = (17.8824f * ct.x) + (43.5161f * ct.y) + (4.11935f * ct.z);
	float3 M = (3.45565f * ct.x) + (27.1554f * ct.y) + (3.86714f * ct.z);
	float3 S = (0.0299566f * ct.x) + (0.184309f * ct.y) + (1.46709f * ct.z);

	// Deuteranope - greens are greatly reduced (1% men)
	float l = 1.0f * L + 0.0f * M + 0.0f * S;
	float m = 0.494207f * L + 0.0f * M + 1.24827f * S;
	float s = 0.0f * L + 0.0f * M + 1.0f * S;

	// LMS to RGB matrix conversion
	couleur.x = (0.0809444479f * l) + (-0.130504409f * m) + (0.116721066f * s);
	couleur.y = (-0.0102485335f * l) + (0.0540193266f * m) + (-0.113614708f * s);
	couleur.z = (-0.000365296938f * l) + (-0.00412161469f * m) + (0.693511405f * s);
	couleur.w = 1;

	return couleur;
}


float4 TritanopePS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;

	ct = textureEntree.Sample(SampleState, tc);

	// RGB to LMS matrix conversion
	float3 L = (17.8824f * ct.x) + (43.5161f * ct.y) + (4.11935f * ct.z);
	float3 M = (3.45565f * ct.x) + (27.1554f * ct.y) + (3.86714f * ct.z);
	float3 S = (0.0299566f * ct.x) + (0.184309f * ct.y) + (1.46709f * ct.z);

	// Tritanope - blues are greatly reduced (0.003% population)
	float l = 1.0f * L + 0.0f * M + 0.0f * S;
	float m = 0.0f * L + 1.0f * M + 0.0f * S;
	float s = -0.395913f * L + 0.801109f * M + 0.0f * S;


	// LMS to RGB matrix conversion
	couleur.x = (0.0809444479f * l) + (-0.130504409f * m) + (0.116721066f * s);
	couleur.y = (-0.0102485335f * l) + (0.0540193266f * m) + (-0.113614708f * s);
	couleur.z = (-0.000365296938f * l) + (-0.00412161469f * m) + (0.693511405f * s);
	couleur.w = 1;

	return couleur;
}


float4 RetroBlurPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;
	float d, dx, dy;

	couleur = 0;
	float x = tc.x * 2 - 1.0;
	float y = tc.y * 2 - 1.0;
	dx = sqrt(x * x);  // Distance du centre
	dy = sqrt(y * y);  // Distance du centre

	dx = x * (distance * dx);  	// Le d?grad? (blur) est en fonction de la
	dy = y * (distance * dy);	 	// distance du centre et de la variable distance.

	x = x - (dx * 10);   // Vous pouvez jouer avec le nombre d'it?rations
	y = y - (dy * 10);

	tc.x = (x + 1.0) / 2.0;
	tc.y = (y + 1.0) / 2.0;

	for (int i = 0; i < 10; i++)   // Vous pouvez jouer avec le nombre d'it?rations
	{
		ct = textureEntree.Sample(SampleState, tc);
		/*couleur = couleur * 0.9 + ct * 0.1;*/ // Vous pouvez ?jouer? avec les %
		couleur = couleur * 0.5 + ct * 0.5; // Vous pouvez ?jouer? avec les %

		x = x + dx;
		y = y + dy;

		tc.x = (x + 1.0) / 2.0;
		tc.y = (y + 1.0) / 2.0;
	}


	// Retro
	couleur = (couleur.x + couleur.y + couleur.z) / 3.0f;
	

	return couleur;
}

float4 ProtanopeBlurPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;

	float d, dx, dy;

	couleur = 0;
	float x = tc.x * 2 - 1.0;
	float y = tc.y * 2 - 1.0;
	dx = sqrt(x * x);  // Distance du centre
	dy = sqrt(y * y);  // Distance du centre

	dx = x * (distance * dx);  	// Le d?grad? (blur) est en fonction de la
	dy = y * (distance * dy);	 	// distance du centre et de la variable distance.

	x = x - (dx * 10);   // Vous pouvez jouer avec le nombre d'it?rations
	y = y - (dy * 10);

	tc.x = (x + 1.0) / 2.0;
	tc.y = (y + 1.0) / 2.0;

	for (int i = 0; i < 10; i++)   // Vous pouvez jouer avec le nombre d'it?rations
	{
		ct = textureEntree.Sample(SampleState, tc);
		/*couleur = couleur * 0.9 + ct * 0.1;*/ // Vous pouvez ?jouer? avec les %
		couleur = couleur * 0.5 + ct * 0.5; // Vous pouvez ?jouer? avec les %

		x = x + dx;
		y = y + dy;

		tc.x = (x + 1.0) / 2.0;
		tc.y = (y + 1.0) / 2.0;
	}

	ct = couleur;

	// RGB to LMS matrix conversion
	float3 L = (17.8824f * ct.x) + (43.5161f * ct.y) + (4.11935f * ct.z);
	float3 M = (3.45565f * ct.x) + (27.1554f * ct.y) + (3.86714f * ct.z);
	float3 S = (0.0299566f * ct.x) + (0.184309f * ct.y) + (1.46709f * ct.z);

	// Protanope - reds are greatly reduced
	float l = 0.0f * L + 2.02344f * M + -2.52581f * S;
	float m = 0.0f * L + 1.0f * M + 0.0f * S;
	float s = 0.0f * L + 0.0f * M + 1.0f * S;


	// LMS to RGB matrix conversion
	couleur.x = (0.0809444479f * l) + (-0.130504409f * m) + (0.116721066f * s);
	couleur.y = (-0.0102485335f * l) + (0.0540193266f * m) + (-0.113614708f * s);
	couleur.z = (-0.000365296938f * l) + (-0.00412161469f * m) + (0.693511405f * s);
	couleur.w = 1;

	return couleur;
}

float4 DeuteranopeBlurPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;
	float d, dx, dy;

	couleur = 0;
	float x = tc.x * 2 - 1.0;
	float y = tc.y * 2 - 1.0;
	dx = sqrt(x * x);  // Distance du centre
	dy = sqrt(y * y);  // Distance du centre

	dx = x * (distance * dx);  	// Le d?grad? (blur) est en fonction de la
	dy = y * (distance * dy);	 	// distance du centre et de la variable distance.

	x = x - (dx * 10);   // Vous pouvez jouer avec le nombre d'it?rations
	y = y - (dy * 10);

	tc.x = (x + 1.0) / 2.0;
	tc.y = (y + 1.0) / 2.0;

	for (int i = 0; i < 10; i++)   // Vous pouvez jouer avec le nombre d'it?rations
	{
		ct = textureEntree.Sample(SampleState, tc);
		/*couleur = couleur * 0.9 + ct * 0.1;*/ // Vous pouvez ?jouer? avec les %
		couleur = couleur * 0.5 + ct * 0.5; // Vous pouvez ?jouer? avec les %

		x = x + dx;
		y = y + dy;

		tc.x = (x + 1.0) / 2.0;
		tc.y = (y + 1.0) / 2.0;
	}

	ct = couleur;

	// RGB to LMS matrix conversion
	float3 L = (17.8824f * ct.x) + (43.5161f * ct.y) + (4.11935f * ct.z);
	float3 M = (3.45565f * ct.x) + (27.1554f * ct.y) + (3.86714f * ct.z);
	float3 S = (0.0299566f * ct.x) + (0.184309f * ct.y) + (1.46709f * ct.z);

	// Deuteranope - greens are greatly reduced (1% men)
	float l = 1.0f * L + 0.0f * M + 0.0f * S;
	float m = 0.494207f * L + 0.0f * M + 1.24827f * S;
	float s = 0.0f * L + 0.0f * M + 1.0f * S;

	// LMS to RGB matrix conversion
	couleur.x = (0.0809444479f * l) + (-0.130504409f * m) + (0.116721066f * s);
	couleur.y = (-0.0102485335f * l) + (0.0540193266f * m) + (-0.113614708f * s);
	couleur.z = (-0.000365296938f * l) + (-0.00412161469f * m) + (0.693511405f * s);
	couleur.w = 1;

	return couleur;
}


float4 TritanopeBlurPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;
	float4 ct;
	float2 tc = vs.CoordTex;
	float d, dx, dy;

	couleur = 0;
	float x = tc.x * 2 - 1.0;
	float y = tc.y * 2 - 1.0;
	dx = sqrt(x * x);  // Distance du centre
	dy = sqrt(y * y);  // Distance du centre

	dx = x * (distance * dx);  	// Le d?grad? (blur) est en fonction de la
	dy = y * (distance * dy);	 	// distance du centre et de la variable distance.

	x = x - (dx * 10);   // Vous pouvez jouer avec le nombre d'it?rations
	y = y - (dy * 10);

	tc.x = (x + 1.0) / 2.0;
	tc.y = (y + 1.0) / 2.0;

	for (int i = 0; i < 10; i++)   // Vous pouvez jouer avec le nombre d'it?rations
	{
		ct = textureEntree.Sample(SampleState, tc);
		/*couleur = couleur * 0.9 + ct * 0.1;*/ // Vous pouvez ?jouer? avec les %
		couleur = couleur * 0.5 + ct * 0.5; // Vous pouvez ?jouer? avec les %

		x = x + dx;
		y = y + dy;

		tc.x = (x + 1.0) / 2.0;
		tc.y = (y + 1.0) / 2.0;
	}

	ct = couleur;

	// RGB to LMS matrix conversion
	float3 L = (17.8824f * ct.x) + (43.5161f * ct.y) + (4.11935f * ct.z);
	float3 M = (3.45565f * ct.x) + (27.1554f * ct.y) + (3.86714f * ct.z);
	float3 S = (0.0299566f * ct.x) + (0.184309f * ct.y) + (1.46709f * ct.z);

	// Tritanope - blues are greatly reduced (0.003% population)
	float l = 1.0f * L + 0.0f * M + 0.0f * S;
	float m = 0.0f * L + 1.0f * M + 0.0f * S;
	float s = -0.395913f * L + 0.801109f * M + 0.0f * S;


	// LMS to RGB matrix conversion
	couleur.x = (0.0809444479f * l) + (-0.130504409f * m) + (0.116721066f * s);
	couleur.y = (-0.0102485335f * l) + (0.0540193266f * m) + (-0.113614708f * s);
	couleur.z = (-0.000365296938f * l) + (-0.00412161469f * m) + (0.693511405f * s);
	couleur.w = 1;



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

technique11 Retro
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 RetroPS();
		SetGeometryShader(NULL);
	}
}

technique11 Protanope
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 ProtanopePS();
		SetGeometryShader(NULL);
	}
};

technique11 Deuteranope
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 DeuteranopePS();
		SetGeometryShader(NULL);
	}
};

technique11 Tritanope
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 TritanopePS();
		SetGeometryShader(NULL);
	}
	
};

technique11 RetroBlur
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 RetroBlurPS();
		SetGeometryShader(NULL);
	}
}

technique11 ProtanopeBlur
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 ProtanopeBlurPS();
		SetGeometryShader(NULL);
	}
};

technique11 DeuteranopeBlur
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 DeuteranopeBlurPS();
		SetGeometryShader(NULL);
	}
};

technique11 TritanopeBlur
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 TritanopeBlurPS();
		SetGeometryShader(NULL);
	}
};
