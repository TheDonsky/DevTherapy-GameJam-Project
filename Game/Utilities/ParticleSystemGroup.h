#pragma once
#include "../Jimara.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::ParticleSystemGroup);

	class ParticleSystemGroup : public virtual Component {
	public:
		inline ParticleSystemGroup(Component* parent, const std::string_view& name = "ToggleActions") : Component(parent, name) {}

		inline virtual ~ParticleSystemGroup() {}

		void SetEmissionRates(float emissionRate) {
			auto particleSystems = GetComponentsInChildren<Jimara::ParticleRenderer>();
			for (const auto& system : particleSystems)
				system->SetEmissionRate(emissionRate);
		}

		virtual void GetSerializedActions(Callback<Jimara::Serialization::SerializedCallback> report) override {
			Component::GetSerializedActions(report);
			{
				static const auto serializer = Jimara::Serialization::DefaultSerializer<float>::Create(
					"Emission Rate", "Particle emission rate");
				report(Jimara::Serialization::SerializedCallback::Create<float>::From(
					"SetEmissionRates", Callback<float>(&ParticleSystemGroup::SetEmissionRates, this), serializer));
			}

		}
	};
}

DT_EXPOSE_COMPONENT_DETAILS(Game::ParticleSystemGroup, "Game/ParticleSystemGroup", "");
