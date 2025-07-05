#pragma once
#include "../Jimara.h"



namespace Game {
	JIMARA_REGISTER_TYPE(Game::CharacterMovementController);
	JIMARA_REGISTER_TYPE(Game::PlayerMovementInput);
	JIMARA_REGISTER_TYPE(Game::PlayerJumpInput);

	class CharacterMovementController : public virtual UpdatingComponent {
	public:
		inline CharacterMovementController(Component* parent, const std::string_view& name = "CharacterMovementController") 
			: Component(parent, name) {}

		inline virtual ~CharacterMovementController() {}

		inline virtual void GetFields(ReportFiedlFn report)override {
			Component::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_rigidbody, "Rigidbody", "Character Rigidbody");
				JIMARA_SERIALIZE_FIELD(m_movementInput, "Movement Input", "World-space movement input on XZ");
				JIMARA_SERIALIZE_FIELD(m_maxVelocity, "Max Velocity", "Maximal movement speed on XZ axis");
				JIMARA_SERIALIZE_FIELD(m_acceleration, "Acceleration", "Velocity change rate");
				JIMARA_SERIALIZE_FIELD(m_jumpInput, "Jump Input", "Jump-request input");
				JIMARA_SERIALIZE_FIELD(m_jumpVelocity, "Jump Velocity", "Jump vertical velocity");
				JIMARA_SERIALIZE_FIELD(m_groundCheck, "Ground Check", "Cround-check input");
			};
		}

	protected:
		inline virtual void Update()override {
			Reference<Rigidbody> body = m_rigidbody;
			if (body == nullptr)
				return;

			Transform* transform = body->GetTransfrom();
			if (transform == nullptr)
				return;

			Vector2 movementInput = InputProvider<Vector2>::GetInput(m_movementInput, Vector2(0.0f));
			if (Math::Magnitude(movementInput) > 1.0f)
				movementInput = Math::Normalize(movementInput);

			bool shouldJump = InputProvider<bool>::GetInput(m_jumpInput, false);
			if (shouldJump)
				shouldJump &= InputProvider<bool>::GetInput(m_groundCheck, true);

			const float deltaTime = Context()->Time()->ScaledDeltaTime();

			Vector3 velocity = body->Velocity();
			velocity = Vector3(
				Math::RateInvariantErp(velocity.x, movementInput.x * m_maxVelocity, deltaTime * m_acceleration),
				shouldJump ? m_jumpVelocity : velocity.y,
				Math::RateInvariantErp(velocity.z, movementInput.y * m_maxVelocity, deltaTime * m_acceleration));

			body->SetVelocity(velocity);

			const float movementAmount = Math::Magnitude(movementInput);
			auto projectDown = [](const auto& dir) { return Math::Normalize(Vector2(dir.x, dir.z)); };
			const Vector2 direction = (movementAmount > std::numeric_limits<float>::epsilon())
				? (movementInput / movementAmount)
				: projectDown(transform->Forward());
			const float curRotation = Math::FloatRemainder(transform->WorldEulerAngles().y, 360.0f);
			const float targetRotation = Math::FloatRemainder(Math::Degrees(std::acos(direction.y) * (direction.x >= 0.0f ? 1.0f : -1.0f)), 360.0f);
			const float deltaA = (targetRotation - curRotation);
			const float deltaB = (deltaA <= 0) ? (360.0f + deltaA) : (deltaA - 360.0f);
			const float delta = ((std::abs(deltaA) < std::abs(deltaB)) ? deltaA : deltaB);
			body->SetAngularVelocity(delta * Math::Min(m_rotationSpeed * 0.25f, 0.25f) / deltaTime * Math::Up());
		}

	private:
		WeakReference<Rigidbody> m_rigidbody;
		WeakReference<InputProvider<Vector2>> m_movementInput;
		float m_maxVelocity = 4.0f;
		float m_acceleration = 4.0f;
		float m_rotationSpeed = 1.0f;
		WeakReference<InputProvider<bool>> m_jumpInput;
		float m_jumpVelocity = 4.0f;
		WeakReference<InputProvider<bool>> m_groundCheck;
	};

	class PlayerMovementInput : public virtual ::Jimara::VectorInput::ComponentFrom<Vector2> {
	public:
		inline PlayerMovementInput(Component* parent, const std::string_view& name = "PlayerMovementInput") : Component(parent, name) {}

		inline virtual ~PlayerMovementInput() {}

		inline virtual void GetFields(ReportFiedlFn report)override {
			Component::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_cameraTransform, "Camera Transform", "");
				JIMARA_SERIALIZE_FIELD(m_deadzone, "Deadzone", "");
			};
		}

		inline virtual std::optional<Vector2> EvaluateInput()override {
			auto getKey = [&](auto key) { return Context()->Input()->KeyPressed(key) ? 1.0f : 0.0f; };
			auto getAxis = [&](auto axis) { return Context()->Input()->GetAxis(axis); };
			const Vector2 rawInput = Vector2(
				getAxis(OS::Input::Axis::CONTROLLER_LEFT_ANALOG_X) + getKey(OS::Input::KeyCode::D) - getKey(OS::Input::KeyCode::A),
				getAxis(OS::Input::Axis::CONTROLLER_LEFT_ANALOG_Y) + getKey(OS::Input::KeyCode::W) - getKey(OS::Input::KeyCode::S));
			if (Math::Magnitude(rawInput) < m_deadzone)
				return Vector2(0.0f);
			Reference<Transform> cameraTransform = m_cameraTransform;
			if (cameraTransform == nullptr)
				return rawInput;
			const Vector3 cameraRight = cameraTransform->Right();
			const Vector3 cameraForward = cameraTransform->Forward();
			auto projectDown = [](const auto& dir) { return Math::Normalize(Vector2(dir.x, dir.z)); };
			return projectDown(cameraRight) * rawInput.x + projectDown(cameraForward) * rawInput.y;
		}

	private:
		WeakReference<Transform> m_cameraTransform;
		float m_deadzone = 0.25f;
	};

	class PlayerJumpInput : public virtual ::Jimara::VectorInput::ComponentFrom<bool> {
	public:
		inline PlayerJumpInput(Component* parent, const std::string_view& name = "PlayerJumpInput") : Component(parent, name) {}

		inline virtual ~PlayerJumpInput() {}

		inline virtual std::optional<bool> EvaluateInput()override {
			auto getKey = [&](auto key) { return Context()->Input()->KeyPressed(key); };
			return getKey(OS::Input::KeyCode::SPACE) || getKey(OS::Input::KeyCode::CONTROLLER_BUTTON_SOUTH);
		};
	};
}

DT_EXPOSE_COMPONENT_DETAILS(::Game::CharacterMovementController, "Game/Player/CharacterMovementController", "");
DT_EXPOSE_COMPONENT_DETAILS(::Game::PlayerMovementInput, "Game/Player/PlayerMovementInput", "");
DT_EXPOSE_COMPONENT_DETAILS(::Game::PlayerJumpInput, "Game/Player/PlayerJumpInput", "");
