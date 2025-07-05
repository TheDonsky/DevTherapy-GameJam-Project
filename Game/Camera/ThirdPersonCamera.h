#pragma once
#include "../Jimara.h"


namespace Game {
	JIMARA_REGISTER_TYPE(::Game::ThirdPersonCamera);
	JIMARA_REGISTER_TYPE(::Game::ThirdPersonCamera::TargetPositionInput);
	JIMARA_REGISTER_TYPE(::Game::ThirdPersonCamera::TargetOffsetDirectionInput);

	class ThirdPersonCamera : public virtual Component {
	public:
		enum class Flags : uint8_t {
			NONE = 0,
			USE_SCALED_TIME =               (1 << 0),
			UPDATE_IN_EDIT_MODE =           (1 << 1),
			SCALE_INPUT_BY_TIME =           (1 << 2),
			KEEP_TRACK_OF_LAST_DISTANCE =   (1 << 3),

			DEFAULT = SCALE_INPUT_BY_TIME
		};

		ThirdPersonCamera(Component* parent, const std::string_view& name = "ThirdPersonCamera");

		virtual ~ThirdPersonCamera();

		virtual void GetFields(ReportFiedlFn report)override;

	protected:
		virtual void OnComponentInitialized()override;

		virtual void OnComponentEnabled()override;

		virtual void OnComponentDisabled()override;

	private:
		WeakReference<Camera> m_camera;
		WeakReference<InputProvider<Vector3>> m_target;
		WeakReference<InputProvider<Vector2>> m_rotationInput;
		
		InputWithDefaultValue<float> m_minPitch = -30.0f;
		InputWithDefaultValue<float> m_maxPitch = 60.0f;

		InputWithDefaultValue<float> m_targetDistance = 1.0f;
		InputWithDefaultValue<Vector2> m_onScreenTargetPosition = Vector2(0.0f);

		InputWithDefaultValue<float> m_minDistance = 0.1f;
		WeakReference<InputProvider<float>> m_clippingDistanceDelayed;
		WeakReference<InputProvider<float>> m_clippingDistanceImmediate;

		InputWithDefaultValue<float> m_clipInSpeed = 1.0f;
		InputWithDefaultValue<float> m_clipOutSpeed = 1.0f;

		Flags m_flags = Flags::DEFAULT;

		float m_lastTargetDistance = std::numeric_limits<float>::quiet_NaN();
		Vector3 m_targetPosition = {};
		Vector3 m_targetOffsetDirection = {};


		struct Helpers;


	public:
		DT_IMPLEMENT_FIELD_INPUT_COMPONENT(TargetPositionInput, ThirdPersonCamera, m_targetPosition);
		DT_IMPLEMENT_FIELD_INPUT_COMPONENT(TargetOffsetDirectionInput, ThirdPersonCamera, m_targetOffsetDirection);
	};

	
	JIMARA_DEFINE_ENUMERATION_BOOLEAN_OPERATIONS(ThirdPersonCamera::Flags);
}

DT_EXPOSE_COMPONENT_DETAILS(::Game::ThirdPersonCamera, "Game/Camera/ThirdPersonCamera", "Third person camera controller");
DT_EXPOSE_COMPONENT_DETAILS(::Game::ThirdPersonCamera::TargetPositionInput,
	"Game/Camera/ThirdPersonCamera/TargetPositionInput", "Third person camera controller's target position input");
DT_EXPOSE_COMPONENT_DETAILS(::Game::ThirdPersonCamera::TargetOffsetDirectionInput,
	"Game/Camera/ThirdPersonCamera/TargetOffsetDirectionInput", "Third person camera controller's target offset direction input");
