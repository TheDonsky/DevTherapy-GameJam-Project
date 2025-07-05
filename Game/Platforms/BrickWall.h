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
        }
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::BrickWall, "Game/Interactable/BrickWall", "", Game::CollisionListener);
