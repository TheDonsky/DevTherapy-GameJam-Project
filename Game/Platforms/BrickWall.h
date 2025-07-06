#pragma once
#include "../CollisionEvents/CollisionListener.h"
#include "../Layers.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::BrickWall);

	class BrickWall : public virtual CollisionListener {
	public:
		inline BrickWall(Component* parent, const std::string_view& name = "BrickWall") : Component(parent, name) {}

		virtual ~BrickWall() {}

		virtual void GetFields(ReportFiedlFn report)override {
			CollisionListener::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_onSmash, "OnSmash", "Hulk Smash");
			};
		}

	protected:
		inline virtual void OnContact(const Collider::ContactInfo& info) override {
			if (info.OtherCollider()->GetLayer() != (Collider::Layer)Layer::CANONBALL)
				return;
			if (info.EventType() != Collider::ContactType::ON_COLLISION_BEGIN)
				return;
            EventCollider()->SetEnabled(false);
            auto component = GetComponentsInChildren<Rigidbody>(true);
            for (auto& c : component) {
                c->SetEnabled(true);
            }
			m_onSmash.Invoke();
        }

	private:
		Jimara::Serialization::SerializedCallback::ProvidedInstance m_onSmash;
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::BrickWall, "Game/Interactable/BrickWall", "", Game::CollisionListener);
