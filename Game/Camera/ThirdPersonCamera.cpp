#include "ThirdPersonCamera.h"


namespace Game {
	struct ThirdPersonCamera::Helpers {
		static void Update(ThirdPersonCamera* self) {
			// Edit-mode check:
			if ((self->m_flags & Flags::UPDATE_IN_EDIT_MODE) == Flags::NONE &&
				(!self->Context()->Updating()))
				return;

			// Camera:
			Reference<Camera> camera = self->m_camera;
			if (camera == nullptr)
				camera = self->GetComponentInParents<Camera>();
			if (camera == nullptr)
				return;

			// Camera-Transform:
			Transform* const transform = camera->GetTransfrom();
			if (transform == nullptr)
				return;

			// Target position:
			const std::optional<Vector3> targetOpt = InputProvider<Vector3>::GetInput(self->m_target);
			if (!targetOpt.has_value())
				return;
			const Vector3& target = targetOpt.value();
			self->m_targetPosition = target;

			// Delta-Time:
			const float deltaTime = ((self->m_flags & Flags::USE_SCALED_TIME) != Flags::NONE)
				? self->Context()->Time()->ScaledDeltaTime()
				: self->Context()->Time()->UnscaledDeltaTime();

			// Apply rotation:
			{
				const Vector2 rawInput = InputProvider<Vector2>::GetInput(self->m_rotationInput, Vector2(0.0f));
				const Vector2 input = ((self->m_flags & Flags::SCALE_INPUT_BY_TIME) != Flags::NONE)
					? (rawInput * deltaTime) : rawInput;

				const float minPitch = self->m_minPitch;
				const float maxPitch = self->m_maxPitch;

				const Vector3 initialRotation = transform->WorldEulerAngles();
				transform->SetLocalEulerAngles(Vector3(
					Math::Min(Math::Max(minPitch, initialRotation.x + input.y), maxPitch),
					Math::FloatRemainder(initialRotation.y + input.x, 360.0f),
					initialRotation.z));
			}

			// Aspect ratio:
			const Reference<const Jimara::RenderStack> renderStack = Jimara::RenderStack::Main(self->Context());
			const Vector2 renderResolution = (renderStack != nullptr) ? renderStack->Resolution() : Size2(0u);
			const float aspectRatio = (renderResolution.y > std::numeric_limits<float>::epsilon()) ? (renderResolution.x / renderResolution.y) : 1.0f;

			// Camera rotation matrix and vectors:
			const Matrix4 rotation = transform->WorldRotationMatrix();
			const Vector3 right = rotation[0];
			const Vector3 up = rotation[1];
			const Vector3 forward = rotation[2];

			// Camera FOV tangents:
			const float tangentY = (camera->Mode() == Camera::ProjectionMode::PERSPECTIVE)
				? std::tan(camera->FieldOfView() * 0.5f)
				: (camera->OrthographicSize() * 0.5f);
			const float tangentX = tangentY * aspectRatio;

			// Direction based on on-screen-position:
			const Vector2 targetOnScreenPosition = self->m_onScreenTargetPosition;
			const Vector3 offsetDirection = -Math::Normalize(forward +
				(targetOnScreenPosition.x * tangentX * right) +
				(targetOnScreenPosition.y * tangentY * up));
			self->m_targetOffsetDirection = offsetDirection;

			// Clipping and distance input:
			const float currentDistance = ((self->m_flags & Flags::KEEP_TRACK_OF_LAST_DISTANCE) == Flags::NONE || std::isnan(self->m_lastTargetDistance))
				? Math::Magnitude(transform->WorldPosition() - target)
				: self->m_lastTargetDistance;
			const float desiredDistance = self->m_targetDistance;
			const float minDistance = self->m_minDistance;
			const float clipDistanceSlow = InputProvider<float>::GetInput(self->m_clippingDistanceDelayed, std::numeric_limits<float>::infinity());
			const float clipDistanceImmediate = InputProvider<float>::GetInput(self->m_clippingDistanceImmediate, std::numeric_limits<float>::infinity());
			const float targetDistance = Math::Max(Math::Min(desiredDistance, clipDistanceSlow, clipDistanceImmediate), minDistance);
			float placementDistance = {};
			
			// Clip-in:
			if (targetDistance < currentDistance) {
				const float clipInSpeed = self->m_clipInSpeed;
				placementDistance = Math::RateInvariantErp(Math::Min(currentDistance, Math::Max(clipDistanceImmediate, minDistance)), targetDistance, deltaTime * clipInSpeed);
			}

			// Clip-out:
			else {
				const float clipOutSpeed = self->m_clipOutSpeed;
				placementDistance = Math::RateInvariantErp(currentDistance, targetDistance, deltaTime * clipOutSpeed);
			}

			// Set camera position:
			transform->SetWorldPosition(target + offsetDirection * placementDistance);
			self->m_lastTargetDistance = placementDistance;
		}

		inline static void OnEnabledOrDisabled(ThirdPersonCamera* self) {
			if (self->ActiveInHierarchy())
				self->Context()->Graphics()->PreGraphicsSynch() += Callback<>(Helpers::Update, self);
			else self->Context()->Graphics()->PreGraphicsSynch() -= Callback<>(Helpers::Update, self);
		}
	};

	ThirdPersonCamera::ThirdPersonCamera(Component* parent, const std::string_view& name)
		: Component(parent, name) {}

	ThirdPersonCamera::~ThirdPersonCamera() {}

	void ThirdPersonCamera::GetFields(ReportFiedlFn report) {
		Component::GetFields(report);
		JIMARA_SERIALIZE_FIELDS(this, report) {
			JIMARA_SERIALIZE_FIELD(m_camera, "Camera", "Camera to control (if not set, the component will try to find one in parent chain)");
			JIMARA_SERIALIZE_FIELD(m_target, "Target", "Target position input");
			JIMARA_SERIALIZE_FIELD(m_rotationInput, "Rotation Input", "Rotation input in degrees");

			JIMARA_SERIALIZE_FIELD(m_minPitch, "Min Pitch", "Minimal camera pitch");
			JIMARA_SERIALIZE_FIELD(m_maxPitch, "Max Pitch", "Maximal camera pitch");

			JIMARA_SERIALIZE_FIELD(m_targetDistance, "Target Distance", "Desired distance to the target point");
			JIMARA_SERIALIZE_FIELD(m_onScreenTargetPosition, "Target On-Screen Position",
				"Target point position in screen-space (with (0; 0) in center and (-1, -1) and (1, 1) in corners)");

			JIMARA_SERIALIZE_FIELD(m_minDistance, "Min Distance",
				"Minimal clipped - in distance from the target point(overrides clip distance inputs if the value happens to be greater than them)");
			JIMARA_SERIALIZE_FIELD(m_clippingDistanceDelayed, "Clip Distance Slow",
				"Clipping distance input that does not require moving the camera closer to the target immediately\n"
				"(maybe based on semi-see-through obstructions or otherwise; Clip-In Speed is used to contol how fast the camera reacts to this input)");
			JIMARA_SERIALIZE_FIELD(m_clippingDistanceImmediate, "Clip Distance Immediate",
				"Clipping distance that requires the camera to get closer to the target immediately without any kind of a delay\n"
				"(maybe based on something like the ground and/or hard walls present in range)");

			JIMARA_SERIALIZE_FIELD(m_clipInSpeed, "Clip-In Speed",
				"Rate at which the camera gets closer to the target when Delayed Clip-Distance reports an obstruction");
			JIMARA_SERIALIZE_FIELD(m_clipOutSpeed, "Clip-Out Speed",
				"Rate at which the camera returns to the desired target distance, once the obstruction reported by either of the Clip-Distance inputs gets removed");

			JIMARA_SERIALIZE_FIELD(m_flags, "Flags", "Flags controlling some miscelanious camera behaviour",
				Object::Instantiate<EnumAttribute<Flags>>(true,
					"USE_SCALED_TIME", Flags::USE_SCALED_TIME,
					"UPDATE_IN_EDIT_MODE", Flags::UPDATE_IN_EDIT_MODE,
					"SCALE_INPUT_BY_TIME", Flags::SCALE_INPUT_BY_TIME,
					"KEEP_TRACK_OF_LAST_DISTANCE", Flags::KEEP_TRACK_OF_LAST_DISTANCE));
		};
	}

	void ThirdPersonCamera::OnComponentInitialized() {
		Helpers::OnEnabledOrDisabled(this);
	}

	void ThirdPersonCamera::OnComponentEnabled() {
		Helpers::OnEnabledOrDisabled(this);
	}

	void ThirdPersonCamera::OnComponentDisabled() {
		Helpers::OnEnabledOrDisabled(this);
	}
}
