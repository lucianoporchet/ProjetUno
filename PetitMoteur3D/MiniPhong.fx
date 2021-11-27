cbuffer param
{
	float4x4 matWorldViewProj; // la matrice totale
	float4x4 matWorld; // matrice de transformation dans le monde
	float4 vLumiere; // la position de la source d’éclairage (Point)
	float4 vCamera; // la position de la caméra
	float4 vAEcl; // la valeur ambiante de l’éclairage
	float4 vAMat; // la valeur ambiante du matériau
	float4 vDEcl; // la valeur diffuse de l’éclairage
	float4 vDMat; // la valeur diffuse du matériau
	float4 vSEcl; // la valeur spéculaire de l’éclairage
	float4 vSMat; // la valeur spéculaire du matériau
	float puissance; // la puissance de spécularité
	int bTex;	// la valeur diffuse du matériau 
	float2 remplissage;
}

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
	float2 coordTex : TEXCOORD3;
};

static VS_Sortie sortie = (VS_Sortie)0;


VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
	
	sortie.Pos = Pos;
	sortie.coordTex = coordTex;

	
	return sortie;
}

Texture2D textureEntree;  // la texture
SamplerState SampleState;  // l'état de sampling

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
	float3 couleur;

	float3 PosWorld = mul(vs.Pos, matWorld).xyz;
	vs.vDirLum = vLumiere.xyz - PosWorld;
	vs.vDirCam = vCamera.xyz - PosWorld;

	//vs.vDirLum.z = -vs.vDirLum.z;



	// Normaliser les paramètres
	float3 N = normalize(vs.Norm);
	float3 L = normalize(vs.vDirLum);
	float3 V = normalize(vs.vDirCam);

	// Valeur de la composante diffuse
	float3 diff = saturate(dot(N, L));

	// R = 2 * (N.L) * N – L
	float3 R = normalize(2 * diff * N - L);

	// Puissance de 4 - pour l'exemple
	float S = pow(saturate(dot(R, V)), puissance);
	float3 couleurTexture;

	if (bTex > 0)
	{
		// Échantillonner la couleur du pixel à partir de la texture
		couleurTexture = textureEntree.Sample(SampleState, vs.coordTex).rgb;
		// I = A + D * N.L + (R.V)n
		//vAEcl.rgb = float3(1.f, 1.f, 1.f);
		couleur = couleurTexture * vAEcl.rgb +
			      couleurTexture * vDEcl.rgb * diff +
		          vSEcl.rgb * vSMat.rgb * S;
	}
	else
	{
		couleur = vAEcl.rgb * vAMat.rgb +
			      vDEcl.rgb * vDMat.rgb * diff +
		          vSEcl.rgb * vSMat.rgb * S;
	}


	return float4(couleur, 1.0f);
}

[maxvertexcount(4)]
void MiniPhongGS(triangle VS_Sortie input[3],
	inout TriangleStream<VS_Sortie> TriStream)
{
	
		// Calculer la normale
		float3 CoteA = input[1].Pos - input[0].Pos;
		float3 CoteB = input[2].Pos - input[0].Pos;
		float3 Normale = normalize(cross(CoteA, CoteB));
		float4 PointSupp = input[0].Pos + (input[2].Pos - input[0].Pos) / 2;
		// Points 0 et 1
		for (int i = 0; i < 2; i++)
		{
			sortie.Pos = mul(input[i].Pos, matWorldViewProj);
			sortie.Norm = -mul(float4(Normale, 0.0f), matWorld).xyz;
			sortie.coordTex = input[i].coordTex;
			TriStream.Append(sortie);
		}
		// Point supplémentaire
		sortie.Pos = mul(PointSupp, matWorldViewProj);
		sortie.Norm = -mul(float4(Normale, 0.0f), matWorld).xyz;
		sortie.coordTex = input[0].coordTex +
			(input[2].coordTex - input[0].coordTex) / 2;
		TriStream.Append(sortie);
		// Point 2
		sortie.Pos = mul(input[2].Pos, matWorldViewProj);
		sortie.Norm = -mul(float4(Normale, 0.0f), matWorld).xyz;
		sortie.coordTex = input[2].coordTex;
	
		TriStream.Append(sortie);
	
}


technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_5_0, MiniPhongPS()));
		SetGeometryShader(CompileShader(gs_5_0, MiniPhongGS()));
		
	}
}
