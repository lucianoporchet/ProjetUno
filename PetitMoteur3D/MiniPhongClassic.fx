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
	int bTex;	// la valeur diffuse du matériau 
	float3 vLumiere2;
	float4 vAEcl2;
	float4 vDEcl2;
	float4 vSEcl2;
	float puissance; // la puissance de spécularité
	float2 remplissage;
}

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 WorldPos: POSITION;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirLum2 : TEXCOORD2;
	float3 vDirCam : TEXCOORD3;
	float2 coordTex : TEXCOORD4;
};




VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

	float3 PosWorld = mul(Pos, matWorld).xyz;
	sortie.WorldPos = mul(Pos, matWorld);

	sortie.vDirLum = vLumiere.xyz - PosWorld;
	//sortie.vDirLum2 = vLumiere2.xyz - PosWorld;
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

	float range = 500.0f;
	float att = 0.00002f;
	// Normaliser les paramètres
	float3 N = normalize(vs.Norm);
	float3 L = normalize(vs.vDirLum);
	float3 V = normalize(vs.vDirCam);

	float3 lightToPixelVec = vLumiere2.xyz - vs.WorldPos.xyz;
	float d = length(lightToPixelVec);
	lightToPixelVec /= d;
	float howMuchLight = dot(lightToPixelVec, N);
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

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
		if (d > range) {
			// I = A + D * N.L + (R.V)n
			couleur = couleurTexture * vAEcl.rgb +
				couleurTexture * vDEcl.rgb * diff +
				vSEcl.rgb * vSMat.rgb * S;
		}
		else  {

			couleur = couleurTexture * (vAEcl.rgb + vAEcl2.rgb / (att * d*d)) +
				couleurTexture * vDEcl.rgb * diff +
				vSEcl.rgb * vSMat.rgb * S;

			//finalColor = vAEcl2.rgb;
			//finalColor /= 0.01f * d;
		}
		//couleur += finalColor;
	}
	else
	{
		if (d > range) {
			couleur = vAEcl.rgb * vAMat.rgb + vDEcl.rgb * vDMat.rgb * diff +
				vSEcl.rgb * vSMat.rgb * S;
		}
		else {

			couleur = (vAEcl.rgb + vAEcl2.rgb / (att * d*d)) * vAMat.rgb + vDEcl.rgb * vDMat.rgb * diff +
				vSEcl.rgb * vSMat.rgb * S;

			//finalColor = vAEcl2.rgb;
			//finalColor /= 0.9f + 0.9f * d;
		}
		//couleur += finalColor;
	}

	//if (PosWorld.y < 0.0f) couleur = float3(1.0f, 0.0f, 0.0f);
	return float4(saturate(couleur), 1.0f);
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