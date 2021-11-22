cbuffer param
{
	float4x4 matWorldViewProj;   // la matrice totale 
	float4x4 matWorld;		// matrice de transformation dans le monde 
	float4 vLumiere; 		// la position de la source d'éclairage (Point)
	float4 vCamera; 			// la position de la caméra
	float4 vAEcl; 			// la valeur ambiante de l'éclairage
	float4 vAMat; 			// la valeur ambiante du matériau
	float4 vDEcl; 			// la valeur diffuse de l'éclairage 
	float4 vDMat; 			// la valeur diffuse du matériau 
}

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
	float2 coordTex : TEXCOORD3;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

	float3 PosWorld = mul(Pos, matWorld).xyz;

	sortie.vDirLum = vLumiere.xyz - PosWorld;
	sortie.vDirCam = vCamera.xyz - PosWorld;

	// Coordonnées d'application de texture
	sortie.coordTex = coordTex;

	return sortie;
}

Texture2D textureEntree;  // la texture

SamplerState SampleState;  // l'état de sampling

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
	float3 couleur;

// Normaliser les paramètres
float3 N = normalize(vs.Norm);
float3 L = normalize(vs.vDirLum);
float3 V = normalize(vs.vDirCam);

// Valeur de la composante diffuse
float3 diff = saturate(dot(N, L));

// R = 2 * (N.L) * N – L
float3 R = normalize(2 * diff * N - L);

// Puissance de 4 - pour l'exemple
float S = pow(saturate(dot(R, V)), 4.0f);

// Échantillonner la couleur du pixel à partir de la texture
float4 couleurTexture = textureEntree.Sample(SampleState, vs.coordTex).rgba;

//float3 couleurFinale;


// I = A + D * N.L + (R.V)n
couleur =	couleurTexture * vAEcl.rgb * vAMat.rgb +
			couleurTexture * vDEcl.rgb * vDMat.rgb * diff;

couleur += S;

return float4(couleur, 1.0f);
}

technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_5_0, MiniPhongPS()));
		SetGeometryShader(NULL);
	}
}
