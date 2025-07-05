#pragma once
#include "../Jimara.h"



namespace Game {
	JIMARA_REGISTER_TYPE(Game::ReturnToGroundIfKillPlaneReached);

	class ReturnToGroundIfKillPlaneReached : public virtual UpdatingComponent {
	public:
		inline ReturnToGroundIfKillPlaneReached(Component* parent, const std::string_view& name = "ReturnToGroundIfKillPlaneReached") : Component(parent, name) {}

		inline ~ReturnToGroundIfKillPlaneReached() {}

		inline virtual void GetFields(ReportFiedlFn report)override {
			Component::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_rigidbody, "Rigidbody", "Character Rigidbody");
				JIMARA_SERIALIZE_FIELD(m_groundCheck, "Ground Check", "Cround-check input");
				JIMARA_SERIALIZE_FIELD(m_groundOffsetOnReturn, "Offset On Return", "Vertical offset on return");
				JIMARA_SERIALIZE_FIELD(m_killPlane, "Kill Plane", "Kill Plane");
			};
		}

	protected:
		inline virtual void Update()override {
			Reference<Rigidbody> body = m_rigidbody;
			if (body == nullptr)
				return;

			Transform* transform = body->GetTransfrom();
			if (transform == nullptr)
				return;

			bool grounded = InputProvider<bool>::GetInput(m_groundCheck, true);

			if (!grounded) {
				if (transform->WorldPosition().y <= m_killPlane && m_lastSafePosition.has_value())
					transform->SetWorldPosition(m_lastSafePosition.value() + Math::Up() * m_groundOffsetOnReturn);
			}
			else m_lastSafePosition = transform->WorldPosition();
		}

	private:
		WeakReference<Rigidbody> m_rigidbody;
		WeakReference<InputProvider<bool>> m_groundCheck;
		float m_groundOffsetOnReturn = 0.5f;
		float m_killPlane = -10.0f;
		std::optional<Vector3> m_lastSafePosition;
	};
}

DT_EXPOSE_COMPONENT_DETAILS(::Game::ReturnToGroundIfKillPlaneReached, "Game/Player/ReturnToGroundIfKillPlaneReached", "");
