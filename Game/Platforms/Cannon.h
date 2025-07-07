#pragma once
#include "GenericPlatform.h"



namespace Game {
	JIMARA_REGISTER_TYPE(Game::Cannon);
	JIMARA_REGISTER_TYPE(Game::CannonThrowOnBallHit);

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
				JIMARA_SERIALIZE_FIELD(m_throwColldown, "Throw cooldown", "Minimal interval between canonball-thows");
				JIMARA_SERIALIZE_FIELD(m_onThrow, "On Trrow", "");
			};
		}

		inline Rigidbody* CannonBall()const { return m_cannonBall.operator Jimara::Reference<Jimara::Rigidbody, Jimara::JimaraReferenceCounter>(); }

		inline void Throw() {
			const float time = Context()->Time()->TotalScaledTime();
			if ((time - m_lastThrowTime) < m_throwColldown)
				return;
			m_lastThrowTime = time;
			Context()->ExecuteAfterUpdate(Callback<Object*>(&Cannon::Throw_callback, this), this);
		}

	protected:
		virtual void OnPlatformEnter() override {
			Throw();
		}

		virtual void OnPlatformExit() override {
		}

	private:
		WeakReference<Transform> m_spawnPoint;
		WeakReference<Rigidbody> m_cannonBall;
		float m_cannonBallSpeed = 32.0f;
		float m_throwColldown = 1.0f;
		Jimara::Serialization::SerializedCallback::ProvidedInstance m_onThrow;

		float m_lastThrowTime = -std::numeric_limits<float>::infinity();

		void Throw_callback(Object*) {
			Reference<Rigidbody> cannonBall = m_cannonBall;
			if (cannonBall == nullptr)
				return;
			Reference<Transform> spawnPoint = m_spawnPoint;
			if (spawnPoint == nullptr)
				return;
			Transform* transform = cannonBall->GetTransfrom();
			if (transform == nullptr)
				return;
			transform->SetWorldPosition(spawnPoint->WorldPosition());
			cannonBall->SetVelocity(spawnPoint->Forward() * m_cannonBallSpeed);
			m_onThrow.Invoke();
		}
	};

	class CannonThrowOnBallHit : public virtual CollisionListener {
	public:
		inline CannonThrowOnBallHit(Component* parent, const std::string_view& name = "CannonThrowOnBallHit") : Component(parent, name) {}

		inline virtual ~CannonThrowOnBallHit() {}
		
		virtual void GetFields(ReportFiedlFn report)override {
			CollisionListener::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_cannon, "Cannon", "Not the camera...");
			};
		}

	protected:
		virtual void OnContact(const Collider::ContactInfo& info) override {
			if (info.OtherCollider()->GetLayer() != (Jimara::Layer)Layer::CANONBALL)
				return;
			Reference<Cannon> cannon = m_cannon;
			if (cannon == nullptr)
				return;
			if (info.OtherCollider()->GetComponentInParents<Rigidbody>() == cannon->CannonBall())
				return;
			cannon->Throw();
		}

	private:
		WeakReference<Cannon> m_cannon;
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::Cannon, "Game/Interactable/Cannon", "", Game::GenericPlatform);
DT_EXPOSE_COMPONENT_DETAILS(Game::CannonThrowOnBallHit, "Game/Interactable/CannonThrowOnBallHit", "", Game::GenericPlatform);
