#pragma once
#include "../Jimara.h"
#include "../Layers.h"



namespace Game {
	JIMARA_REGISTER_TYPE(Game::CollisionListener);
	JIMARA_REGISTER_TYPE(Game::ActionsOnCollision);
	JIMARA_REGISTER_TYPE(Game::SimpleContactFilter);

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
		WeakReference<InputProvider<bool, const Collider::ContactInfo&>> m_contactFilter;
		SerializedCallback::ProvidedInstance m_callback;
	};


	class SimpleContactFilter : public virtual Component, public virtual InputProvider<bool, const Collider::ContactInfo&> {
	public:
		enum class EventTypeMask : uint8_t {
			NONE = 0u,
			ON_COLLISION_BEGIN = (1 << ((int)Jimara::Collider::ContactType::ON_COLLISION_BEGIN)),
			ON_COLLISION_PERSISTS = (1 << ((int)Jimara::Collider::ContactType::ON_COLLISION_PERSISTS)),
			ON_COLLISION_END = (1 << ((int)Jimara::Collider::ContactType::ON_COLLISION_END)),
			ON_TRIGGER_BEGIN = (1 << ((int)Jimara::Collider::ContactType::ON_TRIGGER_BEGIN)),
			ON_TRIGGER_PERSISTS = (1 << ((int)Jimara::Collider::ContactType::ON_TRIGGER_PERSISTS)),
			ON_TRIGGER_END = (1 << ((int)Jimara::Collider::ContactType::ON_TRIGGER_END))
		};
		
		SimpleContactFilter(Component* parent, const std::string_view& name = "ContactFilter");

		virtual ~SimpleContactFilter();

		virtual void GetFields(ReportFiedlFn report)override;

		virtual std::optional<bool> GetInput(const Collider::ContactInfo& info)override;

		virtual void FillWeakReferenceHolder(WeakReferenceHolder& holder) override;
		virtual void ClearWeakReferenceHolder(WeakReferenceHolder& holder) override;

	private:
		Jimara::LayerMask m_layerMask = Jimara::LayerMask::All();
		EventTypeMask m_eventTypeMask = EventTypeMask::NONE;
	};

	JIMARA_DEFINE_ENUMERATION_BOOLEAN_OPERATIONS(SimpleContactFilter::EventTypeMask);
}

namespace Jimara {
	template<> inline static void TypeIdDetails::GetParentTypesOf<::Game::CollisionListener>(const Callback<TypeId>& report) { report(TypeId::Of<Component>()); }
}

DT_EXPOSE_COMPONENT_DETAILS(Game::ActionsOnCollision, "Game/CollisionEvents/ActionsOnCollision",
	"Component that fires serialized events on collider contact events", Game::CollisionListener);

DT_EXPOSE_COMPONENT_DETAILS(Game::SimpleContactFilter, "Game/CollisionEvents/SimpleContactFilter",
	"Simple contact event", Jimara::InputProvider<bool, const Jimara::Collider::ContactInfo&>);
