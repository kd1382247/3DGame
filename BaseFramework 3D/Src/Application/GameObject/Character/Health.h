#pragma once

#include<algorithm>

class Health
{
public:

	// 最大HPをセットしてHPを満タンにする
	void Init(const int maxHP)
	{
		m_maxHP = maxHP;
		m_hp = maxHP;
	}
		
	// HPを減らす。0以下になるとtrueを返す
	bool TakeDamage(const int damage)
	{
		m_hp -= damage;
		if (m_hp <= 0)
		{
			m_hp = 0;
			return true;
		}

		return false;
	}

	// HP回復
	void Heal(const int amount)
	{
		m_hp += amount;
		if (m_hp >= m_maxHP)
		{
			m_hp = m_maxHP;
		}
	}

	// HPが0かどうか
	bool IsDead() const
	{
		return m_hp <= 0;
	}

	int GetCurrentHP()const { return m_hp; }
	int GetMaxHP()    const { return m_maxHP; }

	float GetHPRate() const
	{
		// 最大HPが0の場合は0を返す
		if (m_maxHP <= 0)
		{
			return 0;
		}

		float hpRate = std::clamp( static_cast<float>(m_hp) / static_cast<float>(m_maxHP),0.0f,1.0f);

		return hpRate;
	}

private:

	int m_hp    = 0;
	int m_maxHP = 0;

};