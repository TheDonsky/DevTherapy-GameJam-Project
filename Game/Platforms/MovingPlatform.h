#pragma once
#include "../Jimara.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::MovingPlatform);

	class MovingPlatform : public virtual UpdatingComponent {
	public:
		inline MovingPlatform(Component* parent, const std::string_view& name = "MovingPlatform") : Component(parent, name) {}

		inline virtual ~MovingPlatform() {}

		virtual void GetFields(ReportFiedlFn report)override {
			UpdatingComponent::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_platformTransform, "Platform", "Platform Transform");
				JIMARA_SERIALIZE_FIELD(m_poses, "Poses", "Pose transforms");
				JIMARA_SERIALIZE_FIELD_GET_SET(Target, SetTarget, "Target", "");
			};
		}

		size_t Target()const { return m_currentTarget; }

		void SetTarget(size_t poseIndex) {
			if (m_currentTarget == poseIndex)
				return;
			m_phase = -1.0f;
			if (poseIndex >= m_poses.size())
				return;
			Reference<Transform> platform = m_platformTransform;
			if (platform == nullptr)
				return;
			m_initialPos = platform->WorldPosition();
			m_initialRotation = platform->WorldEulerAngles();
			m_currentTarget = poseIndex;
			m_phase = 0.0f;
		}

		void MoveToNextTarget() {
			if (m_poses.size() > 0u)
				SetTarget((m_currentTarget + 1u) % m_poses.size());
		}

		void MoveToPrevTarget() {
			if (m_poses.size() > 0u)
				SetTarget((m_currentTarget + m_poses.size() - 1u) % m_poses.size());
		}

	protected:
		inline virtual void Update()override {
			if (m_phase < 0.0f)
				return;

			Reference<Transform> platform = m_platformTransform;
			if (platform == nullptr)
				return;

			if (m_currentTarget >= m_poses.size())
				return;

			Reference<Transform> targetPose = m_poses[m_currentTarget];
			if (targetPose == nullptr)
				return;

			m_phase = (m_interpolationTime <= std::numeric_limits<float>::epsilon()) ? 1.0f
				: Math::Min(m_phase + Context()->Time()->ScaledDeltaTime() / m_interpolationTime, 1.0f);

			const Vector3 targetPosition = targetPose->WorldPosition();
			const Vector3 targetEulerAngles = targetPose->WorldEulerAngles();

			platform->SetWorldPosition(Math::Lerp(m_initialPos, targetPosition, m_phase));
			platform->SetWorldEulerAngles(Vector3(
				Math::LerpAngles(m_initialRotation.x, targetEulerAngles.x, m_phase),
				Math::LerpAngles(m_initialRotation.y, targetEulerAngles.y, m_phase),
				Math::LerpAngles(m_initialRotation.z, targetEulerAngles.z, m_phase)));

			if (std::abs(m_phase - 1.0f) < std::numeric_limits<float>::epsilon())
				m_phase = -1.0f;
		}

	private:
		WeakReference<Transform> m_platformTransform;
		std::vector<WeakReference<Transform>> m_poses;
		float m_interpolationTime = 2.0f;
		
		float m_phase = -1.0f;
		size_t m_currentTarget = ~size_t(0u);
		Vector3 m_initialPos = Vector3(0.0f);
		Vector3 m_initialRotation = Vector3(0.0f);
	};
}


DT_EXPOSE_COMPONENT_DETAILS(Game::MovingPlatform, "Game/Platforms/MovingPlatform", "", Game::UpdatingComponent);
