#pragma once

// アニメーションキー(クォータニオン
struct KdAnimKeyQuaternion
{
	float				m_time = 0;		// 時間
	Math::Quaternion	m_quat;			// クォータニオンデータ
};

// アニメーションキー(ベクトル
struct KdAnimKeyVector3
{
	float				m_time = 0;		// 時間
	Math::Vector3		m_vec;			// 3Dベクトルデータ
};

// 1ノード分の姿勢。拡縮、回転、位置の３つを持つ
struct NodePose
{
	Math::Vector3    m_scale = Math::Vector3::One;
	Math::Quaternion m_rotation = {};
	Math::Vector3    m_translation = {};
};

//============================
// アニメーションデータ
//============================
struct KdAnimationData
{
	// アニメーション名
	std::string		m_name;
	// アニメの長さ
	float			m_maxLength = 0;

	// １ノードのアニメーションデータ
	struct Node
	{
		int			m_nodeOffset = -1;	// 対象モデルノードのOffset値

		// 各チャンネル
		std::vector<KdAnimKeyVector3>		m_translations;	// 位置キーリスト
		std::vector<KdAnimKeyQuaternion>	m_rotations;	// 回転キーリスト
		std::vector<KdAnimKeyVector3>		m_scales;		// 拡縮キーリスト

		void Interpolate(Math::Matrix& rDst, float time);
		void InterpolateBlend(Math::Matrix& rDst, float time, const NodePose& pose, float t);
		bool InterpolateTranslations(Math::Vector3& result, float time);
		bool InterpolateRotations(Math::Quaternion& result, float time);
		bool InterpolateScales(Math::Vector3& result, float time);
	};

	// 全ノード用アニメーションデータ
	std::vector<Node>	m_nodes;
};

class KdAnimator
{
public:

	inline void SetAnimation(const std::shared_ptr<KdAnimationData>& rData, bool isLoop = true,float blendTime=0.0f)
	{

		// 切り替える前にアニメが入っていたか
		bool hadAnimation = m_spAnimation != nullptr;

		m_spAnimation = rData;
		m_isLoop = isLoop;


		// ブレンドするのか
		if (blendTime > 0 && hadAnimation)
		{
			m_blendDuration = blendTime;
			m_needSnapshot = true;
		}
		else
		{
			m_blendDuration = 0.0f;
			m_needSnapshot = false;
		}
		
		m_blendElapsed = 0.0f;

		m_time = 0.0f;
	}

	// アニメーションが終了してる？
	bool IsAnimationEnd() const
	{
		if (m_spAnimation == nullptr) { return true; }
		if (m_time >= m_spAnimation->m_maxLength) { return true; }

		return false;
	}

	// アニメーションがループしている？
	bool IsAnimationLoop()const
	{
		if (m_spAnimation == nullptr) { return true; }
		return m_isLoop;
	}

	// アニメーションの更新
	void AdvanceTime(std::vector<KdModelWork::Node>& rNodes, float speed = 1.0f,float deltaTime=0.0f);

private:

	std::shared_ptr<KdAnimationData>	m_spAnimation = nullptr;	// 再生するアニメーションデータ

	float m_time = 0.0f;

	bool m_isLoop = false;

	// アニメを切り替えた際の固定した姿勢を格納
	std::vector<NodePose> m_blendFromPoses;

	// ブレンドにかける時間
	float m_blendDuration = 0.0f;

	// ブレンドを始めてからの経過時間
	float m_blendElapsed = 0.0f;

	// AdvanceTimeで写真をとるという合図
	bool  m_needSnapshot = false;


};
