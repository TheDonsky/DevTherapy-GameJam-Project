#pragma once
#include "../Jimara.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::AudioVolumeFade);

	class AudioVolumeFade : public virtual UpdatingComponent {
	public:
		inline AudioVolumeFade(Component* parent, const std::string_view& name = "AudioVolumeFade") : Component(parent, name) {}

		inline virtual ~AudioVolumeFade() {}

		inline float TargetVolume()const { return m_targetVolume; }

		inline void SetTargetVolume(float volume) { m_targetVolume = Math::Max(volume, 0.0f); }

		virtual void GetFields(ReportFiedlFn report)override {
			Component::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_audioSource, "Audio Source", "Audio Source");
				JIMARA_SERIALIZE_FIELD_GET_SET(TargetVolume, SetTargetVolume, "Target Volume", "Volume to fade towards");
				JIMARA_SERIALIZE_FIELD(m_fadeSpeed, "Fade speed", "Volume change rate");
			};
		}

		inline virtual void GetSerializedActions(Callback<Jimara::Serialization::SerializedCallback> report) override {
			Component::GetSerializedActions(report);
			{
				static const auto serializer = Jimara::Serialization::DefaultSerializer<float>::Create(
					"Target Volume", "Volume to fade towards");
				report(Jimara::Serialization::SerializedCallback::Create<float>::From(
					"SetTargetVolume", Callback<float>(&AudioVolumeFade::SetTargetVolume, this), serializer));
			}
		}


	protected:
		inline virtual void Update()override {
			Reference<AudioSource> source = m_audioSource;
			if (source == nullptr)
				return;
			source->SetVolume(Math::RateInvariantErp(source->Volume(), m_targetVolume, Context()->Time()->ScaledDeltaTime() * m_fadeSpeed));
		}

	private:
		WeakReference<AudioSource> m_audioSource;
		float m_targetVolume = 1.0f;
		float m_fadeSpeed = 1.0f;
	};
}


DT_EXPOSE_COMPONENT_DETAILS(Game::AudioVolumeFade, "Game/Audio/AudioVolumeFade", "", Game::UpdatingComponent);
