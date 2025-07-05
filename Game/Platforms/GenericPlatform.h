#pragma once
#include "../CollisionEvents/CollisionListener.h"
#include "../Layers.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::GenericPlatform);

	class GenericPlatform : public virtual CollisionListener {
	public:
		virtual ~GenericPlatform() {}

		virtual void GetFields(ReportFiedlFn report)override {
			CollisionListener::GetFields(report);
		}

	protected:
		inline virtual void OnContact(const Collider::ContactInfo& info) override {
			if (info.OtherCollider()->GetLayer() != (Collider::Layer)Layer::PLAYER)
				return;
			if (info.EventType() == Collider::ContactType::ON_TRIGGER_BEGIN)
				OnPlatformEnter();
			else if (info.EventType() == Collider::ContactType::ON_TRIGGER_END)
				OnPlatformExit();
		}

		virtual void OnPlatformEnter() = 0;
		virtual void OnPlatformExit() = 0;
	};
}

namespace Jimara {
	template<> inline static void TypeIdDetails::GetParentTypesOf<::Game::GenericPlatform>(const Callback<TypeId>& report) { report(TypeId::Of<Game::CollisionListener>()); }
}
