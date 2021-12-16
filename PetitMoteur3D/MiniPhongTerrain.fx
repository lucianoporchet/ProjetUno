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

static float3 WorldPos;

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = mul(Pos, matWorldViewProj);
	WorldPos = mul(Pos, matWorld);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

	float3 PosWorld = mul(Pos, matWorld).xyz;

	sortie.vDirLum = vLumiere.xyz - PosWorld;
	sortie.vDirCam = vCamera.xyz - PosWorld;

	// Coordonnées d'application de texture
	sortie.coordTex = coordTex;

	return sortie;
}

Texture2D textureEntree[8];  // la texture
SamplerState SampleState;    // l'état de sampling

static float3 lum1 = float3(398.0f, 5821.0f, -422.0f);
static float3 lum2 = float3(31.0f, 5689.0f, 98.0f);
static float3 lum3 = float3(-336.0f, 5514.0f, 528.0f);
static float3 lum4 = float3(459.0f, 6244.0f, 353.0f);
static float3 lum5 = float3(-334.0f, 5224.0f, -127.0f);
static float3 coulLum = float3(1.0f, 0.0f, 0.0f);


float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
	float3 couleur;
	float range = 500.0f;
	float att = 0.000025f;
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
	// L'on utilise la 3e texture de filtre pour determiner quelle texture apposer sur notre objet
	float3 couleurTexture = textureEntree[1].Sample(SampleState, vs.coordTex * 8).rgb * textureEntree[2].Sample(SampleState, vs.coordTex).rgb.x
	+ textureEntree[0].Sample(SampleState, vs.coordTex * 8).rgb * (1 - textureEntree[2].Sample(SampleState, vs.coordTex).rgb.x);

	float3 lightToPixelVec = lum1 - WorldPos.xyz;
	float d1 = length(lightToPixelVec);

	lightToPixelVec = lum2 - WorldPos.xyz;
	float d2 = length(lightToPixelVec);

	lightToPixelVec = lum3 - WorldPos.xyz;
	float d3 = length(lightToPixelVec);

	lightToPixelVec = lum4 - WorldPos.xyz;
	float d4 = length(lightToPixelVec);

	lightToPixelVec = lum5 - WorldPos.xyz;
	float d5 = length(lightToPixelVec);

	float3 totalAbiant = vAEcl.rgb;

	if (d1 <= range) {
		totalAbiant += coulLum.rgb / (att * d1 * d1);
	}
	if (d2 <= range) {
		totalAbiant += coulLum.rgb / (att * d2 * d2);
	}
	if (d3 <= range) {
		totalAbiant += coulLum.rgb / (att * d3 * d3);
	}
	if (d4 <= range) {
		totalAbiant += coulLum.rgb / (att * d4 * d4);
	}
	if (d5 <= range) {
		totalAbiant += coulLum.rgb / (att * d5 * d5);
	}
	
	// I = A + D * N.L + (R.V)n
	couleur = couleurTexture * totalAbiant * vAMat.rgb +
			   couleurTexture * vDEcl.rgb * vDMat.rgb * diff;

	couleur += S;

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
