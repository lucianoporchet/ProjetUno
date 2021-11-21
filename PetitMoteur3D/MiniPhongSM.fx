cbuffer param
{ 
	float4x4 matWorldViewProj;   // la matrice totale 
	float4x4 matWorldViewProjLight; // Matrice WVP pour lumière
	float4x4 matWorld;	// matrice de transformation dans le monde 
	float4 vLumiere; // la position de la source d'éclairage (Point)
	float4 vCamera; 	// la position de la caméra
	float4 vAEcl; 		// la valeur ambiante de l'éclairage
	float4 vAMat; 	// la valeur ambiante du matériau
	float4 vDEcl; 		// la valeur diffuse de l'éclairage 
	float4 vDMat; 			// la valeur diffuse du matériau 
	float4 vSEcl; 			// la valeur spéculaire de l'éclairage 
	float4 vSMat; 			// la valeur spéculaire du matériau 
	float puissance;
	int bTex;		    	// Booléen pour la présence de texture
	float2 remplissage;
}

struct ShadowMapVS_SORTIE
{
	float4 Pos : SV_POSITION;
	float3 Profondeur:	TEXCOORD0;
};

//-------------------------------------------------------------------
// Vertex Shader pour construire le shadow map
//-------------------------------------------------------------------
ShadowMapVS_SORTIE ShadowMapVS( float4 Pos : POSITION )
{
	ShadowMapVS_SORTIE Out = (ShadowMapVS_SORTIE)0;

	// Calcul des coordonnées
	Out.Pos = mul( Pos, matWorldViewProjLight);  // WVP de la lumiere

	// Obtenir la profondeur et normaliser avec w
	Out.Profondeur.x = Out.Pos.z / Out.Pos.w ; 

	return Out;
}

//-------------------------------------------------------------------
// Pixel Shader pour le shadow map
//-------------------------------------------------------------------
float4 ShadowMapPS( ShadowMapVS_SORTIE entree ) : SV_Target
{
	// retourner la profondeur normalisée [0..1]
	return entree.Profondeur.x;
}

RasterizerState rsCullFront
{
	CullMode = Front;
};

//-------------------------------------------------------------------
// Technique pour le shadow map
//-------------------------------------------------------------------
technique11 ShadowMap 
{
	pass p0 
    {
		VertexShader = compile vs_5_0 ShadowMapVS();
		SetRasterizerState( rsCullFront );
		
 		PixelShader = compile ps_5_0 ShadowMapPS();
		SetGeometryShader(NULL);
	}
}

Texture2D textureEntree;    // la texture
SamplerState SampleState;   // l'état de sampling

Texture2D ShadowTexture;	// La texture du shadow map
SamplerState ShadowMapSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp; 
    AddressV = Clamp;
};

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
	float2 coordTex : TEXCOORD3; 
	float4 PosInMap : TEXCOORD4;
};

//-----------------------------------------------------
// Vertex Shader miniphong avec shadow map
//-----------------------------------------------------
VS_Sortie MiniPhongVS(float4 Pos : POSITION, 
                      float3 Normale : NORMAL, float2 coordTex: TEXCOORD0)  
{
	VS_Sortie sortie = (VS_Sortie)0;
	
	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;
	
	float3 PosWorld = mul(Pos, matWorld).xyz;

	sortie.vDirLum = vLumiere.xyz - PosWorld;
	sortie.vDirCam = vCamera.xyz - PosWorld;

	// Coordonnées d'application de texture
	sortie.coordTex = coordTex;

	// Valeurs pour shadow map
	// Coordonnées
	sortie.PosInMap = mul(Pos, matWorldViewProjLight);

	return sortie;
}

#define DIMTEX 512

float4 MiniPhongPS( VS_Sortie vs ) : SV_Target
{
	float3 couleur;

	// Normaliser les paramètres
	float3 N = normalize(vs.Norm);
	float3 L = normalize(vs.vDirLum);
	float3 V = normalize(vs.vDirCam);

	// Valeur de la composante diffuse
	float3 diff = saturate(dot(N, L)); 

	// R = 2 * (N.L) * N - L
	float3 R = normalize(2 * diff * N - L);

	// Calcul de la spécularité 
	float3 S = pow(saturate(dot(R, V)), puissance);

	float3 couleurTexture;

	if (bTex>0)
	{
		// Échantillonner la couleur du pixel à partir de la texture
		couleurTexture = textureEntree.Sample(SampleState, vs.coordTex).rgb;
	}
	else
	{
		couleurTexture = vDMat.rgb;
	}

	// Calculer la valeur du shadowmapping
	// Calculer les coordonnées de texture (ShadowMap)
	float2 ShadowCoord = 0.5f * vs.PosInMap.xy / vs.PosInMap.w + float2(0.5f, 0.5f);
	ShadowCoord.y = 1.0f - ShadowCoord.y;
 
	float2 PixelActuel = DIMTEX * ShadowCoord; // Pour une texture de 512 X 512

	// Valeur de l'interpolation linéaire
	float2 lerps = frac( PixelActuel );

	// Lire les valeurs du tableau, avec les vérifications de profondeur
	float Profondeur = vs.PosInMap.z / vs.PosInMap.w ; 
	float3 kernel[9];

	float echelle = 1.0/DIMTEX;

	float2 coord[9];
	coord[0] = ShadowCoord + float2(-echelle,-echelle);
	coord[1] = ShadowCoord + float2(-echelle, 0.0    );
	coord[2] = ShadowCoord + float2(-echelle, echelle);
	coord[3] = ShadowCoord + float2( 0.0    ,-echelle);
	coord[4] = ShadowCoord + float2( 0.0    , 0.0    );
	coord[5] = ShadowCoord + float2( 0.0    , echelle);
	coord[6] = ShadowCoord + float2( echelle,-echelle);
	coord[7] = ShadowCoord + float2( echelle, 0.0    );
	coord[8] = ShadowCoord + float2( echelle, echelle);

	// Colonne 1
	kernel[0].x = (ShadowTexture.Sample(ShadowMapSampler, coord[0]).r < Profondeur) ? 0.0f: 1.0f;
	kernel[0].y = (ShadowTexture.Sample(ShadowMapSampler, coord[1]).r < Profondeur) ? 0.0f: 1.0f;
	kernel[0].z = (ShadowTexture.Sample(ShadowMapSampler, coord[2]).r < Profondeur) ? 0.0f: 1.0f;
	// Colonne 2
	kernel[1].x = (ShadowTexture.Sample(ShadowMapSampler, coord[3]).r < Profondeur) ? 0.0f: 1.0f;
	kernel[1].y = (ShadowTexture.Sample(ShadowMapSampler, coord[4]).r < Profondeur) ? 0.0f: 1.0f;
	kernel[1].z = (ShadowTexture.Sample(ShadowMapSampler, coord[5]).r < Profondeur) ? 0.0f: 1.0f;
	// Colonne 3
	kernel[2].x = (ShadowTexture.Sample(ShadowMapSampler, coord[6]).r < Profondeur) ? 0.0f: 1.0f;
	kernel[2].y = (ShadowTexture.Sample(ShadowMapSampler, coord[7]).r < Profondeur) ? 0.0f: 1.0f;
	kernel[2].z = (ShadowTexture.Sample(ShadowMapSampler, coord[8]).r < Profondeur) ? 0.0f: 1.0f;
 
 	// Les interpolations linéaires
	// Interpoler colonnes 1 et 2
	float3 col12 = lerp( kernel[0], kernel[1], lerps.x );
	// Interpoler colonnes 2 et 3
	float3 col23 = lerp( kernel[1], kernel[2], lerps.x );
 	// Interpoler ligne 1 et colonne 1
	float4 lc;
           lc.x = lerp( col12.x, col12.y, lerps.y );
 	// Interpoler ligne 2 et colonne 1
           lc.y = lerp( col12.y, col12.z, lerps.y );
 	// Interpoler ligne 1 et colonne 2
           lc.z = lerp( col23.x, col23.y, lerps.y );
	// Interpoler ligne 2 et colonne 1
           lc.w = lerp( col23.y, col23.z, lerps.y );

	// Faire la moyenne
	float ValeurOmbre = (lc.x + lc.y + lc.z + lc.w) / 4.0;

 	// I = A + D * N.L + (R.V)n
	couleur =	(couleurTexture * vAEcl.rgb +
				couleurTexture * vDEcl.rgb * diff.rgb +
				vSEcl.rgb * vSMat.rgb * S)   * ValeurOmbre;

	return float4(couleur, 1.0f);;
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
