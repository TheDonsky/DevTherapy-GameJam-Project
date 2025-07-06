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
				JIMARA_SERIALIZE_FIELD(m_rotationAmount, "Rotation Amount", "");
				JIMARA_SERIALIZE_FIELD(m_onRotationStarted, "On Rotation Started", "");
				JIMARA_SERIALIZE_FIELD(m_onRotationFinished, "On Rotation Finished", "");
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
			const float deltaA = (m_targetAngle.value() - Math::FloatRemainder(eulerAngles.y, 360.0f));
			const float deltaB = (deltaA <= 0) ? (360.0f + deltaA) : (deltaA - 360.0f);
			const float totalDelta = ((std::abs(deltaA) < std::abs(deltaB)) ? deltaA : deltaB);

			const float delta = Math::Min(std::abs(totalDelta), std::abs(deltaTime * m_rotationSpeed)) * (totalDelta >= 0.0f ? 1.0f : -1.0f);
			eulerAngles = Vector3(eulerAngles.x, eulerAngles.y + delta, eulerAngles.z);
			if (std::abs(m_rotationSpeed) < 0.000001f || std::abs(Math::FloatRemainder(eulerAngles.y - m_targetAngle.value(), 360.0f)) < 0.01f) {
				eulerAngles.y = m_targetAngle.value();
				m_targetAngle = std::nullopt;
				m_onRotationFinished.Invoke();
			}

			target->SetWorldEulerAngles(eulerAngles);
		}

		virtual void OnPlatformEnter() override {
			Reference<Transform> target = m_targetTransform;
			if (target == nullptr)
				return;
			const float curTarget = Math::FloatRemainder(m_targetAngle.has_value() ? m_targetAngle.value() : target->WorldEulerAngles().y, 360.0f);
			m_targetAngle = Math::FloatRemainder(curTarget + m_rotationAmount, 360.0f);
			m_onRotationStarted.Invoke();
		}

		virtual void OnPlatformExit() override {

		}

	private:
		WeakReference<Transform> m_targetTransform;
		std::optional<float> m_targetAngle;
		float m_rotationSpeed = 45.0f;
		float m_rotationAmount = 90.0f;
		Jimara::Serialization::SerializedCallback::ProvidedInstance m_onRotationStarted;
		Jimara::Serialization::SerializedCallback::ProvidedInstance m_onRotationFinished;
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::RotatePlatform, "Game/Platforms/RotatePlatform", "", Game::RotatePlatform);
