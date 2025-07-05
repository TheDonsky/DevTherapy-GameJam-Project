#pragma once
#include "../Jimara.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::ActionsOnEnabledOrDisabled);

	class ActionsOnEnabledOrDisabled : public virtual Component {
	public:
		inline ActionsOnEnabledOrDisabled(Component* parent, const std::string_view& name = "ToggleActions") : Component(parent, name) {}

		inline virtual ~ActionsOnEnabledOrDisabled() {}

		virtual void GetFields(ReportFiedlFn report)override {
			Component::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_onEnabled, "OnEnabled", "Invoked when enabled");
				JIMARA_SERIALIZE_FIELD(m_onDisabled, "OnDisabled", "Invoked when disabled");
			};
		}

	protected:
		virtual void OnComponentEnabled()override {
			PerformActionList();
		}

		virtual void OnComponentDisabled()override {
			PerformActionList();
		}

		inline void PerformActionList() {
			const std::vector<SerializedCallback::ProvidedInstance>& actions = ActiveInHierarchy() ? m_onEnabled : m_onDisabled;
			for (size_t i = 0u; i < actions.size(); i++)
				actions[i].Invoke();
		}

	private:
		std::vector<SerializedCallback::ProvidedInstance> m_onEnabled;
		std::vector<SerializedCallback::ProvidedInstance> m_onDisabled;
	};
}


DT_EXPOSE_COMPONENT_DETAILS(Game::ActionsOnEnabledOrDisabled, "Game/ActionsOnEnabledOrDisabled", "");
