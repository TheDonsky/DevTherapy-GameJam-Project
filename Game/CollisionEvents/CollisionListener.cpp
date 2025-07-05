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
		if (InputProvider<bool, const Collider::ContactInfo&>::GetInput(m_contactFilter, info, true))
			m_callback.Invoke();
	}
}
