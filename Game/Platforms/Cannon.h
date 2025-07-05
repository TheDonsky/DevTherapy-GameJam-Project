#pragma once
#include "GenericPlatform.h"



namespace Game {
	JIMARA_REGISTER_TYPE(Game::Cannon);

	class Cannon : public virtual GenericPlatform {
	public:
		inline Cannon(Component* parent, const std::string_view& name = "Cannon") : Component(parent, name) {}

		virtual ~Cannon() {}

		virtual void GetFields(ReportFiedlFn report)override {
			GenericPlatform::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_spawnPoint, "Spawn Point", "Origin coordinates for object spawning");
				JIMARA_SERIALIZE_FIELD(m_cannonBall, "Cannon Ball", "Rigidbody of the cannon ball");
				JIMARA_SERIALIZE_FIELD(m_cannonBallSpeed, "Cannon Ball Speed", "Speed of the cannon ball");
			};
		}

	protected:
		virtual void OnPlatformEnter() override {
			Reference<Rigidbody> cannonBall = m_cannonBall;
			if (cannonBall == nullptr)
				return;
			Reference<Transform> spawnPoint = m_spawnPoint;
			if (spawnPoint == nullptr)
				return;
			Transform* transform = cannonBall->GetTranform();
			if (transform == nullptr)
				return;
			transform->SetWorldPosition(spawnPoint->WorldPosition());
			cannonBall->SetLinearVelocity(spawnPoint->Forward() * m_cannonBallSpeed);
		}

		virtual void OnPlatformExit() override {

		}

	private:
		WeakReference<Transform> m_spawnPoint;
		WeakReference<Rigidbody> m_cannonBall;
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::Cannon, "Game/Interactable/Cannon", "", Game::Cannon);
