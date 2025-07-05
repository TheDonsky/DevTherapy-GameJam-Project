#pragma once
#include "GenericPlatform.h"



namespace Game {
	JIMARA_REGISTER_TYPE(Game::WindEffector);

	class WindEffector : public virtual CollisionListener {
	public:
		inline WindEffector(Component* parent, const std::string_view& name = "Cannon") : Component(parent, name) {}

		virtual ~WindEffector() {}

		virtual void GetFields(ReportFiedlFn report)override {
			CollisionListener::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_windForce, "Wind Force", "Force, applied to any rigidbody within the range");
			};
		}

	protected:
		inline virtual void OnContact(const Collider::ContactInfo& info) override {
			if (info.EventType() != Collider::ContactType::ON_TRIGGER_PERSISTS)
				return;
			Rigidbody* body = info.OtherCollider()->GetComponentInParents<Rigidbody>();
			if (body != nullptr)
				Context()->ExecuteAfterUpdate(Callback<Object*>(&WindEffector::ApplyForce, this), body);
		}

	private:
		Vector3 m_windForce = Vector3(0.0f, 0.0f, 10.0f);

		void ApplyForce(Object* bodyPtr) {
			Rigidbody* body = dynamic_cast<Rigidbody*>(bodyPtr);
			body->AddForce(m_windForce * body->Mass());
		}
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::WindEffector, "Game/Effectors/WindEffector", "", Game::CollisionListener);
