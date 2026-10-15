#include "TPSCamera.h"

#include"../../../System/CollisionManager/CollisionManager.h"

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


	using ObjectList = std::vector<std::weak_ptr<KdGameObject>>;
	using ObstacleLists = std::vector<const ObjectList*>;

	// カメラが当たる対象(AABB・OBB)のリストを集める
	ObstacleLists GetObstacleLists()
	{
		ObstacleLists lists;

		lists.push_back(&CollisionManager::Instance().GetObjects(CollisionLayer::AABB));
		lists.push_back(&CollisionManager::Instance().GetObjects(CollisionLayer::OBB));

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
}

void TPSCamera::PostUpdate()
{
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
	UpdateRotateByMouse();
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
