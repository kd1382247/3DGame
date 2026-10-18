#include "TPSCamera.h"

#include"../../../System/CollisionManager/CollisionManager.h"

#include"../../Gimmick/RoomGate/RoomGate.h"
#include"../../../System/TimeManager/TimeManager.h"

namespace
{
	// レイの起点にする、プレイヤーの足元からの高さ
	constexpr float kPivotHeight = 0.5f;
	// 壁とカメラの間に確保する余白
	constexpr float kWallMargin = 0.4f;
	// カメラ用スフィアの半径
	constexpr float kCameraRadius = 0.3f;
	// カメラの戻る速さ
	constexpr float kReturnSpeed = 0.1f;
	// 追う速さ
	constexpr float kFollowSpeed = 0.3f;

	// カメラが対象に到着した判定
	constexpr float kArriveThreshold = 0.99f; // Approachの到着判定
	constexpr float kLeaveThreshold = 0.01f;  // Returnの到着判定

	using ObjectList = std::vector<std::weak_ptr<KdGameObject>>;
	using ObstacleLists = std::vector<const ObjectList*>;

	// カメラが当たる対象(AABB・OBB)のリストを集める
	ObstacleLists GetObstacleLists()
	{
		ObstacleLists lists;

		lists.push_back(&CollisionManager::Instance().GetObjects(CollisionLayer::AABB));
		lists.push_back(&CollisionManager::Instance().GetObjects(CollisionLayer::OBB));
		lists.push_back(&CollisionManager::Instance().GetObjects(CollisionLayer::Gate));
		return lists;
	}

	// カメラが当たる全オブジェクトに対して func を呼ぶ
	template<class Func>
	void ForEachObstacle(Func&& func)
	{
		for (const auto* objList : GetObstacleLists())
		{
			for (const auto& wpGameObj : *objList)
			{
				std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
				if (spGameObj)
				{
					func(*spGameObj);
				}
			}
		}
	}
}

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 2.0f, -4.5f);

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);


	SetObjectCategory(ObjectCategory::Camera);

	// オブジェクト名セット
	SetObjectName("TPSCamera");

	m_cameraDist = -1.0f;

	m_smoothTargetPos = {};
	m_isSmoothTargetInit = false;


	m_introState = BossIntroState::None;
	m_introBlend = 0.0f;
	m_shakeTime = 0.0f;
	m_isDefeat = false;

}

void TPSCamera::PostUpdate()
{

	UpdateBossIntro();

	const std::shared_ptr<const KdGameObject> spTarget = m_wpTarget.lock();

	// カメラの回転とワールド行列の更新
	UpdateCameraMatrix(spTarget);

	// ターゲットがいない場合は補正しない
	if (!spTarget)
	{
		return;
	}

	// ↓めり込み防止の為の座標補正↓
	// 壁の向こう側に回り込まないよう、プレイヤー側へ引き寄せる
	PullInByRay(*spTarget);

	// 壁に近づきすぎないよう、壁から押し戻す
	PushOutBySphere();

	ApplyBossIntro();

}

void TPSCamera::StartBossIntro(const Math::Vector3& pos, const BossIntroParam& param)
{
	const std::shared_ptr<const KdGameObject> spTarget = m_wpTarget.lock();
	
	// プレイヤーがいない
	// m_introStateがNone以外の時は受け付けない
	if (!spTarget||
		m_introState!=BossIntroState::None)
	{
		return;
	}

	// 引数を保存
	m_bossPos = pos;
	m_introParam = param;
	m_isDefeat = false;

	// カメラの位置を計算
	Math::Vector3 bossDir = spTarget->GetPos() - m_bossPos ;

	bossDir.y = 0.0f;
	// 長さがほぼ0なら、借りの方向に差し替える
	if (bossDir.LengthSquared() <= 0.000001f)
	{
		bossDir = Math::Vector3(0.0f, 0.0f, -1.0f);
	}

	bossDir.Normalize();

	// カメラの位置をボスから見たプレイヤーの位置の方向に
	// distance分離れた位置に持っていく
	m_introCamPos = m_bossPos + bossDir * m_introParam.distance + Math::Vector3(0.0f, m_introParam.height, 0.0f);

	// カメラの向きをセット
	m_introCamRot = CreateLookRotation(m_introCamPos, m_introParam.lookHeight);


	// ステートを変更しシェイクタイマーを0にする
	m_introState = BossIntroState::Approach;
	m_shakeTime = 0.0f;

}

void TPSCamera::StartBossDefeat(const Math::Vector3& pos, const Math::Vector3& forward, const BossIntroParam& param)
{
	const std::shared_ptr<const KdGameObject> spTarget = m_wpTarget.lock();

	// プレイヤーがいない
	// m_introStateがNone以外の時は受け付けない
	if (!spTarget ||
		m_introState != BossIntroState::None)
	{
		return;
	}

	// 引数を保存し、撃破演出であることを覚える
	m_bossPos = pos;
	m_introParam = param;
	m_isDefeat = true;

	// ボスの正面の向き(水平方向だけ)
	Math::Vector3 bossFront = forward;

	bossFront.y = 0.0f;
	// 長さがほぼ0なら、仮の方向に差し替える
	if (bossFront.LengthSquared() <= 0.000001f)
	{
		bossFront = Math::Vector3(0.0f, 0.0f, -1.0f);
	}

	bossFront.Normalize();

	// カメラの基準の位置をボスの正面の方向に
	// distance分離れた位置に持っていく
	m_introCamPos = m_bossPos + bossFront * m_introParam.distance + Math::Vector3(0.0f, m_introParam.height, 0.0f);

	// カメラの向きをセット
	m_introCamRot = CreateLookRotation(m_introCamPos, m_introParam.lookHeight);

	// ステートを変更しタイマーを0にする
	m_introState = BossIntroState::Approach;
	m_shakeTime = 0.0f;

}

void TPSCamera::UpdateBossIntro()
{
	if (m_introState == BossIntroState::None)
	{
		return;
	}

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	switch (m_introState)
	{
	case TPSCamera::BossIntroState::Approach:

		// 少しずつ1へ近づける
		m_introBlend += (1.0f - m_introBlend) * m_introParam.approachSpeed;

		if (m_introBlend >= kArriveThreshold)
		{
			m_introBlend = 1.0f;
			// 撃破演出ならそのままよろけ演出へ、咆哮演出なら合図を待つ
			m_introState = m_isDefeat ? BossIntroState::Stagger : BossIntroState::Hold;
		}

		break;
	case TPSCamera::BossIntroState::Hold:

		break;
	case TPSCamera::BossIntroState::Roar:

		m_shakeTime += deltaTime;
		
		if (m_shakeTime >= m_introParam.shakeDuration)
		{
			m_introState = BossIntroState::Return;
		}

		break;
	case TPSCamera::BossIntroState::Stagger:

		m_shakeTime += deltaTime;

		if (m_shakeTime >= m_introParam.staggerDuration)
		{
			m_introState = BossIntroState::Return;
		}

		break;
	case TPSCamera::BossIntroState::Return:

		// 撃破演出では、戻る間も動きが途切れないよう時間を進め続ける
		if (m_isDefeat)
		{
			m_shakeTime += deltaTime;
		}

		m_introBlend += (0.0f - m_introBlend) * m_introParam.returnSpeed;

		if (m_introBlend <= kLeaveThreshold)
		{
			m_introBlend = 0.0f;
			m_introState = BossIntroState::None;
		}

		break;
	}
}

void TPSCamera::ApplyBossIntro()
{
	if (m_introBlend <= 0)
	{
		return;
	}

	// 今フレームの、演出カメラの目標の位置と向き
	// (咆哮演出では、StartBossIntroで決めた位置と向きのまま)
	Math::Vector3    introPos = m_introCamPos;
	Math::Quaternion introRot = m_introCamRot;

	// 撃破演出では、ボスの正面を中心に左右へ回り込む
	if (m_isDefeat)
	{
		// 基準の位置を、ボスを中心にY軸まわりで回す
		// (m_introCamPosそのものは書き換えない)
		Math::Vector3    offset = m_introCamPos - m_bossPos;
		Math::Quaternion sway   = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::Up, CalcSwayAngle());

		offset = Math::Vector3::Transform(offset, sway);

		introPos = m_bossPos + offset;

		// 回した位置から、ボスを見る向きを作り直す
		introRot = CreateLookRotation(introPos, m_introParam.lookHeight);
	}

	// 位置を混ぜる
	Math::Vector3 camPos = GetPos();
	Math::Vector3 blendPos = {};

	blendPos = Math::Vector3::Lerp(camPos,introPos,m_introBlend);

	// 回転を混ぜる
	Math::Quaternion camRotation = Math::Quaternion::CreateFromRotationMatrix(m_mRotation);
	Math::Quaternion blendRotation = {};

	blendRotation = Math::Quaternion::Slerp(camRotation, introRot, m_introBlend);

	m_mWorld = Math::Matrix::CreateFromQuaternion(blendRotation);

	Math::Vector3 camFoward = Math::Vector3::TransformNormal(Math::Vector3::Backward, m_mWorld);

	blendPos += camFoward * CalcShakeOffsest();

	// 撃破演出の小刻みな揺れ(カメラの左右方向)
	// 戻るとき(Return)は、混ぜ具合に合わせて小さくして、最後に飛ばないようにする
	Math::Vector3 camRight = Math::Vector3::TransformNormal(Math::Vector3::Right, m_mWorld);

	blendPos += camRight * CalcStaggerShake() * m_introBlend;

	SetPos(blendPos);

}

float TPSCamera::CalcShakeOffsest() const
{

	// Roarじゃいないまたは、揺れの時間が0の場合は0を返す
	if (m_introState != BossIntroState::Roar||
		m_introParam.shakeDuration<=0)
	{
		return 0.0f;
	}

	// 進み具合
	float t = std::clamp( m_shakeTime / m_introParam.shakeDuration,0.0f,1.0f);

	// 徐々に弱める
	float decay = 1 - t;

	// カメラの揺れ
	float angle = m_shakeTime * m_introParam.shakeFrequency * DirectX::XM_2PI;

	// サインカーブで揺らす
	float wave = sin(angle);

	return m_introParam.shakeAmplitude * wave * decay;
}

float TPSCamera::CalcSwayAngle() const
{
	// 撃破演出でなければ、回り込まない
	if (!m_isDefeat)
	{
		return 0.0f;
	}

	// 回り込む角度の上限(ラジアン)
	float maxAngle = DirectX::XMConvertToRadians(m_introParam.swayAngle);

	// ゆっくりしたサインカーブで、左右に往復させる
	float wave = sin(m_shakeTime * m_introParam.swayFrequency * DirectX::XM_2PI);

	return maxAngle * wave;
}

float TPSCamera::CalcStaggerShake() const
{
	// 撃破演出でない、または揺れの設定が0以下の場合は0を返す
	if (!m_isDefeat ||
		m_introParam.staggerShakeInterval <= 0.0f ||
		m_introParam.staggerShakeBurst <= 0.0f)
	{
		return 0.0f;
	}

	// 今の揺れが始まってからの時間(0 〜 staggerShakeInterval)
	float phase = std::fmod(m_shakeTime, m_introParam.staggerShakeInterval);

	// 揺れの時間が終わったら、次の揺れが始まるまで0
	if (phase >= m_introParam.staggerShakeBurst)
	{
		return 0.0f;
	}

	// 進み具合
	float t = phase / m_introParam.staggerShakeBurst;

	// 徐々に弱める
	float decay = 1.0f - t;

	// 小刻みな揺れ
	float angle = phase * m_introParam.staggerShakeFrequency * DirectX::XM_2PI;

	// サインカーブで揺らす
	float wave = sin(angle);

	return m_introParam.staggerShakeAmplitude * wave * decay;
}

Math::Quaternion TPSCamera::CreateLookRotation(const Math::Vector3& camPos, const float lookHeight)
{

	Math::Vector3 lookDir = m_bossPos + Math::Vector3(0.0f,lookHeight, 0.0f);

	Math::Vector3 camDir = lookDir - camPos;

	// カメラと見る点がほぼ同じ位置の場合、Identityを返す
	if (camDir.LengthSquared() <= 0.000001f)
	{
		return Math::Quaternion::Identity;
	}

	// 正規化
	camDir.Normalize();

	// カメラの向くそれぞれの軸を作る
	Math::Vector3 forward = camDir;

	Math::Vector3 right   = Math::Vector3::Up.Cross(forward);
	right.Normalize();

	Math::Vector3 up      = forward.Cross(right);

	Math::Matrix rotation(right, up, forward);

	return Math::Quaternion::CreateFromRotationMatrix(rotation);
}

void TPSCamera::UpdateCameraMatrix(const std::shared_ptr<const KdGameObject>& spTarget)
{
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix targetMat = Math::Matrix::Identity;
	if (spTarget)
	{
		// カメラの位置がセットされていない場合はセットする
		if (!m_isSmoothTargetInit)
		{
			m_smoothTargetPos = spTarget->GetPos();
			m_isSmoothTargetInit = true;
		}
		else
		{
			m_smoothTargetPos = Math::Vector3::Lerp(m_smoothTargetPos, spTarget->GetPos(), kFollowSpeed);
		}

		targetMat = Math::Matrix::CreateTranslation(m_smoothTargetPos);
	}

	// カメラの回転
	if(m_introState==BossIntroState::None)
	{
		UpdateRotateByMouse();
	}
	m_mRotation = GetRotationMatrix();
	m_mWorld = m_mLocalPos * m_mRotation * targetMat;
}

void TPSCamera::PullInByRay(const KdGameObject& target)
{
	// 補正前の「本来のカメラ位置」を保存しておく
	const Math::Vector3 desiredPos = GetPos();

	// プレイヤーからカメラに向けてレイを飛ばし、
	// プレイヤーに一番近い壁でカメラを止める
	KdCollider::RayInfo rayInfo;

	// レイの発射位置(プレイヤーの足元から少し上)
	rayInfo.m_pos = target.GetPos() + Math::Vector3(0.0f, kPivotHeight, 0.0f);

	// レイの発射方向(プレイヤー → カメラ)と長さ
	rayInfo.m_dir = desiredPos - rayInfo.m_pos;
	rayInfo.m_range = rayInfo.m_dir.Length();
	rayInfo.m_dir.Normalize();

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround | KdCollider::TypeBump;

	// レイに当たったリストから一番近いオブジェクトを検出
	float maxOverLap = 0.0f;
	Math::Vector3 hitPos = {};
	bool hit = false;

	ForEachObstacle([&](KdGameObject& obj)
	{
		std::list<KdCollider::CollisionResult> retRayList;
		obj.Intersects(rayInfo, &retRayList);

		for (const auto& ret : retRayList)
		{
			// レイを遮断しオーバーした長さが
			// 一番長いものを探す
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				hitPos = ret.m_hitPos;
				hit = true;
			}
		}
	});

	
	float targetDist = 0.0f;

	if (hit)
	{
		// 何かしらの障害物に当たっている
		// プレイヤーから壁までの距離を求め、余白ぶん手前(プレイヤー側)で止める
		const float hitDist = (hitPos - rayInfo.m_pos).Length();
		targetDist = hitDist - kWallMargin;
		if (targetDist < 0.0f) { targetDist = 0.0f; }
	}
	else
	{
		targetDist = rayInfo.m_range;
	}

	if (m_cameraDist<0 || m_cameraDist>targetDist)
	{
		m_cameraDist = targetDist;
	}
	else
	{
		m_cameraDist += (targetDist - m_cameraDist) * kReturnSpeed;
	}

	SetPos(rayInfo.m_pos + rayInfo.m_dir * m_cameraDist);
}

void TPSCamera::PushOutBySphere()
{
	// カメラのスフィアを作る
	KdCollider::SphereInfo sphere;

	sphere.m_sphere.Center = GetPos();
	sphere.m_sphere.Radius = kCameraRadius;
	sphere.m_type = KdCollider::TypeBump;

	// 押し戻り量
	float         sphereOverlap = 0.0f;
	Math::Vector3 pushDir = {};
	bool          sphereHit = false;

	// Sphereとボックスの判定をする
	ForEachObstacle([&](KdGameObject& obj)
	{
		std::list<KdCollider::CollisionResult> retSphereList;
		obj.Intersects(sphere, &retSphereList);

		for (const auto& ret : retSphereList)
		{
			if (sphereOverlap < ret.m_overlapDistance)
			{
				sphereOverlap = ret.m_overlapDistance;
				pushDir = ret.m_hitDir;
				sphereHit = true;
			}
		}
	});

	if (sphereHit)
	{
		SetPos(GetPos() + pushDir * sphereOverlap);
	}
}
