#pragma once
#include "../CollisionEvents/CollisionListener.h"
#include "../Layers.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::BrickWallPlatform);

	class BrickWallPlatform : public virtual CollisionListener {
	public:
		virtual ~BrickWallPlatform() {}

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

namespace Jimara {
	template<> inline static void TypeIdDetails::GetParentTypesOf<::Game::BrickWallPlatform>(const Callback<TypeId>& report) { report(TypeId::Of<Game::CollisionListener>()); }
}
