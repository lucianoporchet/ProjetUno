cbuffer param
{
	float4x4 matWorldViewProj;   // la matrice totale 
	float4x4 matWorld;		// matrice de transformation dans le monde 
	float4 vLumiere; 		// la position de la source d'�clairage (Point)
	float4 vCamera; 			// la position de la cam�ra
	float4 vAEcl; 			// la valeur ambiante de l'�clairage
	float4 vAMat; 			// la valeur ambiante du mat�riau
	float4 vDEcl; 			// la valeur diffuse de l'�clairage 
	float4 vDMat; 			// la valeur diffuse du mat�riau 
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
	//sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;
	sortie.Norm = Normale;
	float3 PosWorld = mul(Pos, matWorld).xyz;
	// Coordonn�es d'application de texture
	sortie.coordTex = coordTex;

	return sortie;
}


Texture2D up;
Texture2D down;
Texture2D left;
Texture2D right;
Texture2D back;
Texture2D front;

SamplerState SampleState;  // l'�tat de sampling

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
	float3 couleur;
	float3 N = normalize(vs.Norm);
	float4 couleurTexture;
	
	if (N.x > 0.5f) {
		couleurTexture = left.Sample(SampleState, vs.coordTex).rgba;
	}
	else if(N.x < -0.5f){
		couleurTexture = right.Sample(SampleState, vs.coordTex).rgba;
	}
	else if (N.y > 0.5f) {
		couleurTexture = up.Sample(SampleState, vs.coordTex).rgba;
	}
	else if (N.y < -0.5f) {
		couleurTexture = down.Sample(SampleState, vs.coordTex).rgba;
	}
	else if (N.z > 0.5f) {
		couleurTexture = back.Sample(SampleState, vs.coordTex).rgba;
	}
	else {
		couleurTexture = front.Sample(SampleState, vs.coordTex).rgba;
	}

	couleur = couleurTexture.rgb + couleurTexture.rgb;

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
