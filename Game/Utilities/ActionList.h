#pragma once
#include "../Jimara.h"


namespace Game {
	JIMARA_REGISTER_TYPE(Game::ActionList);

	class ActionList : public virtual Component {
	public:
		inline ActionList(Component* parent, const std::string_view& name = "ActionList") : Component(parent, name) {}

		inline virtual ~ActionList() {}

		virtual void GetFields(ReportFiedlFn report)override {
			Component::GetFields(report);
			JIMARA_SERIALIZE_FIELDS(this, report) {
				JIMARA_SERIALIZE_FIELD(m_actions, "Actions", "Action list, invoked when InvokeActions() is called");
			};
		}

		inline virtual void GetSerializedActions(Callback<Jimara::Serialization::SerializedCallback> report) override {
			Component::GetSerializedActions(report);
			{
				report(Jimara::Serialization::SerializedCallback::Create<>::From(
					"InvokeActions", Callback<>(&ActionList::InvokeActions, this)));
			}
		}

		inline void InvokeActions() {
			for (size_t i = 0u; i < m_actions.size(); i++)
				m_actions[i].Invoke();
		}

	private:
		std::vector<SerializedCallback::ProvidedInstance> m_actions;
	};


}

DT_EXPOSE_COMPONENT_DETAILS(Game::ActionList, "Game/ActionList", "");
