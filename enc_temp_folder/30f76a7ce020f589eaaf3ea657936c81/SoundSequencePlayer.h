#pragma once
#include "../Jimara.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::SoundSequencePlayer);

	class SoundSequencePlayer : public virtual Component {
	public:
		inline SoundSequencePlayer(Component* parent, const std::string_view& name = "SoundSequencePlayer") : Component(parent, name) {}

		inline virtual ~SoundSequencePlayer() {}

		virtual void GetFields(ReportFiedlFn report)override {
			Component::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_audioSource, "Audio Source", "Audio Source");
				JIMARA_SERIALIZE_FIELD(m_clipSequence, "Clip Sequence", "Clips to play in order");
				JIMARA_SERIALIZE_FIELD(m_currentClip, "Current Clip", "Current clip index");
			};
		}

		virtual void GetSerializedActions(Callback<Jimara::Serialization::SerializedCallback> report) override {
			Component::GetSerializedActions(report);
			{
				report(Jimara::Serialization::SerializedCallback::Create<>::From(
					"Play", Callback<>(&SoundSequencePlayer::Play, this)));
			}
		}

		inline void Play() {
			if (m_clipSequence.size() <= 0u)
				return;
			m_currentClip %= m_clipSequence.size();
			Reference<Jimara::Audio::AudioClip> clip = m_clipSequence[m_currentClip];
			m_currentClip++;
			Reference<AudioSource> source = m_audioSource;
			if (source != nullptr && clip != nullptr)
				source->PlayOneShot(clip);
		}

	private:
		WeakReference<AudioSource> m_audioSource;
		std::vector<Reference<Jimara::Audio::AudioClip>> m_clipSequence;
		size_t m_currentClip = 0u;
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::SoundSequencePlayer, "Game/Audio/SoundSequencePlayer", "");
