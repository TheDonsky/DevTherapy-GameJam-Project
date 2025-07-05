#pragma once
#include "../Jimara.h"



namespace Game {
	JIMARA_REGISTER_TYPE(Game::CollisionListener);
	JIMARA_REGISTER_TYPE(Game::ActionsOnCollision);

	class CollisionListener : public virtual Component {
	public:
		virtual ~CollisionListener() = 0;

		Collider* EventCollider()const;

		void SetEventCollider(Collider* collider);

		virtual void GetFields(ReportFiedlFn report)override;

	protected:
		virtual void OnContact(const Collider::ContactInfo& info) = 0;

	private:
		SpinLock m_colliderChangeLock;
		Reference<Collider> m_collider;
	};


	class ActionsOnCollision : public virtual CollisionListener {
	public:
		ActionsOnCollision(Component* parent, const std::string_view& name = "ActionsOnCollision");

		virtual ~ActionsOnCollision();

		virtual void GetFields(ReportFiedlFn report)override;

	protected:
		virtual void OnContact(const Collider::ContactInfo& info) override;

	private:
		SerializedCallback::ProvidedInstance m_callback;
	};
}

namespace Jimara {
	template<> inline static void TypeIdDetails::GetParentTypesOf<::Game::CollisionListener>(const Callback<TypeId>& report) { report(TypeId::Of<Component>()); }
}

DT_EXPOSE_COMPONENT_DETAILS(Game::ActionsOnCollision, "Game/CollisionEvents/ActionsOnCollision",
	"Component that fires serialized events on collider contact events", Game::CollisionListener);
