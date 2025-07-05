#pragma once
#include "GenericPlatform.h"



namespace Game {
	JIMARA_REGISTER_TYPE(Game::RotatePlatform);

	class RotatePlatform : public virtual GenericPlatform, public virtual UpdatingComponent {
	public:
		inline RotatePlatform(Component* parent, const std::string_view& name = "RotatePlatform") : Component(parent, name) {}

		virtual ~RotatePlatform() {}

		virtual void GetFields(ReportFiedlFn report)override {
			GenericPlatform::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_targetTransform, "Target Transform", "");
				JIMARA_SERIALIZE_FIELD(m_rotationSpeed, "Rotation Speed", "");
			};
		}

	protected:
		virtual void Update() override {
			if (!m_targetAngle.has_value())
				return;
			Reference<Transform> target = m_targetTransform;
			if (target == nullptr)
				return;
			
			const float deltaTime = Context()->Time()->ScaledDeltaTime();
			
			Vector3 eulerAngles = target->WorldEulerAngles();
			float totalDelta = (m_targetAngle.value() - eulerAngles.y);

			const float delta = Math::Min(std::abs(totalDelta), std::abs(deltaTime * m_rotationSpeed)) * (totalDelta >= 0.0f ? 1.0f : 0.0f);
			eulerAngles = Vector3(eulerAngles.x, eulerAngles.y + delta, eulerAngles.z);
			if (std::abs(eulerAngles.y - m_targetAngle.value()) < 0.01f) {
				eulerAngles.y = m_targetAngle.value();
				m_targetAngle = std::nullopt;
			}

			target->SetWorldEulerAngles(eulerAngles);
		}

		virtual void OnPlatformEnter() override {
			Reference<Transform> target = m_targetTransform;
			if (target == nullptr)
				return;
			m_targetAngle = (m_targetAngle.has_value() ? m_targetAngle.value() : target->WorldEulerAngles().y) + 90.0f;
		}

		virtual void OnPlatformExit() override {

		}

	private:
		WeakReference<Transform> m_targetTransform;
		std::optional<float> m_targetAngle;
		float m_rotationSpeed = 45.0f;
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::RotatePlatform, "Game/Platforms/RotatePlatform", "", Game::RotatePlatform);
