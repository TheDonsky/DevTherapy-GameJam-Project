#include "CollisionListener.h"


namespace Game {
	CollisionListener::~CollisionListener() {
		assert(EventCollider() == nullptr);
	}

	Collider* CollisionListener::EventCollider()const {
		return m_collider;
	}

	void CollisionListener::SetEventCollider(Collider* collider) {
		std::unique_lock<SpinLock> lock(m_colliderChangeLock);

		if (collider == m_collider || Destroyed())
			return;

		struct Events {
			static void ClearCollider(CollisionListener* self) {
				if (self->m_collider == nullptr)
					return;
				self->m_collider->OnDestroyed() -= Callback<Component*>(Events::OnColliderDestroyed, self);
				self->m_collider->OnContact() -= Callback<const Collider::ContactInfo&>(Events::OnCollisionEvent, self);
				self->OnDestroyed() -= Callback<Component*>(Events::OnSelfDestroyed);
				self->m_collider = nullptr;
			}

			static void OnSelfDestroyed(Component* selfPtr) {
				CollisionListener* self = dynamic_cast<CollisionListener*>(selfPtr);
				assert(self != nullptr);
				std::unique_lock<SpinLock> lock(self->m_colliderChangeLock);
				ClearCollider(self);
			}

			static void OnColliderDestroyed(CollisionListener* self, Component* colliderObj) {
				std::unique_lock<SpinLock> lock(self->m_colliderChangeLock);
				assert(self->m_collider == colliderObj);
				ClearCollider(self);
			}

			static void OnCollisionEvent(CollisionListener* self, const Collider::ContactInfo& info) {
				assert(self->m_collider == info.ReportingCollider());
				self->OnContact(info);
			}
		};

		Events::ClearCollider(this);
		
		m_collider = collider;
		if (m_collider == nullptr)
			return;
		m_collider->OnDestroyed() += Callback<Component*>(Events::OnColliderDestroyed, this);
		m_collider->OnContact() += Callback<const Collider::ContactInfo&>(Events::OnCollisionEvent, this);
		OnDestroyed() += Callback<Component*>(Events::OnSelfDestroyed);
	}

	void CollisionListener::GetFields(ReportFiedlFn report) {
		Component::GetFields(report);
		JIMARA_SERIALIZE_FIELDS(this, report) {
			JIMARA_SERIALIZE_FIELD_GET_SET(EventCollider, SetEventCollider, "Event Collider", "Collider, to listen to for the contact events");
		};
	}

	ActionsOnCollision::ActionsOnCollision(Component* parent, const std::string_view& name) 
		: Component(parent, name) {}

	ActionsOnCollision::~ActionsOnCollision() {}

	void ActionsOnCollision::GetFields(ReportFiedlFn report) {
		CollisionListener::GetFields(report);
		JIMARA_SERIALIZE_FIELDS(this, report) {
			JIMARA_SERIALIZE_FIELD(m_contactFilter, "Contact Filter", "");
			JIMARA_SERIALIZE_FIELD(m_callback, "Action", "Serialized action that will get invoked when OnContact gets fired");
		};
	}

	void ActionsOnCollision::OnContact(const Collider::ContactInfo& info) {
		if (InputProvider<bool, const Collider::ContactInfo&>::GetInput(m_contactFilter, info, true)) {
			struct Invoker {
				static void Invoke(Object* selfPtr) {
					dynamic_cast<ActionsOnCollision*>(selfPtr)->m_callback.Invoke();
				}
			};
			Context()->ExecuteAfterUpdate(Callback<Object*>(Invoker::Invoke), this);
		}
	}


	SimpleContactFilter::SimpleContactFilter(Component* parent, const std::string_view& name) : Component(parent, name) {}

	SimpleContactFilter::~SimpleContactFilter() {}

	void SimpleContactFilter::GetFields(ReportFiedlFn report) {
		Component::GetFields(report);
		JIMARA_SERIALIZE_FIELDS(this, report) {
			JIMARA_SERIALIZE_FIELD(m_layerMask, "Layer mask", "Relevant contact layers", Jimara::Layers::LayerMaskAttribute::Instance());
			JIMARA_SERIALIZE_FIELD(m_eventTypeMask, "Events", "Event Type Mask",
				Object::Instantiate<EnumAttribute<EventTypeMask>>(true,
					"ON_COLLISION_BEGIN", EventTypeMask::ON_COLLISION_BEGIN,
					"ON_COLLISION_PERSISTS", EventTypeMask::ON_COLLISION_PERSISTS,
					"ON_COLLISION_END", EventTypeMask::ON_COLLISION_END,
					"ON_TRIGGER_BEGIN", EventTypeMask::ON_TRIGGER_BEGIN,
					"ON_TRIGGER_PERSISTS", EventTypeMask::ON_TRIGGER_PERSISTS,
					"ON_TRIGGER_END", EventTypeMask::ON_TRIGGER_END));
		};
	}

	std::optional<bool> SimpleContactFilter::GetInput(const Collider::ContactInfo& info) {
		if (!m_layerMask[info.OtherCollider()->GetLayer()])
			return false;
		if ((static_cast<EventTypeMask>(1 << (uint8_t)info.EventType()) & m_eventTypeMask) == EventTypeMask::NONE)
			return false;
		return true;
	}

	void SimpleContactFilter::FillWeakReferenceHolder(WeakReferenceHolder& holder) {
		Component::FillWeakReferenceHolder(holder);
	}
	void SimpleContactFilter::ClearWeakReferenceHolder(WeakReferenceHolder& holder) {
		Component::ClearWeakReferenceHolder(holder);
	}
}
