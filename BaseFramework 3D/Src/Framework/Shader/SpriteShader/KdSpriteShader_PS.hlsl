#include "inc_KdSpriteShader.hlsli"

// テクスチャ
Texture2D g_inputTex : register(t0);
// サンプラ
SamplerState g_ss : register(s0);


//============================================
// 2D描画 ピクセルシェーダ
//============================================
float4 main(VSOutput In) : SV_Target0
{
	// テクスチャ色取得
	float4 texCol = g_inputTex.Sample(g_ss, In.UV);
	//アルファテスト
	if (texCol.a < 0.1f) discard;

	// シーケンシャルシャイン
	{
		if (g_shineEnable)
		{

			// 斜めの位置 t で距離を測る
			float t = In.UV.x + In.UV.y * g_shineSlope;
			
			// 帯の中心から、このピクセルまでの距離
			float dist = abs(t - g_shine.x);

			// 距離→帯の強さ(中心1で、幅の端で0、それより外は0)
			float band = saturate(1 - dist / g_shine.y);

			// 明るさを足す
			texCol.rgb += texCol.rgb * band * g_shine.z;
		}
	}

	// テクスチャ色 * 指定色
	return texCol * g_color;
	
}
