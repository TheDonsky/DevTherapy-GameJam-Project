#pragma once
#include <Jimara/Environment/Scene/Scene.h>
#include <Jimara/Environment/Layers.h>
#include <Jimara/Data/Serialization/Helpers/SerializerMacros.h>
#include <Jimara/Data/Serialization/Attributes/EnumAttribute.h>
#include <Jimara/Data/Serialization/Attributes/SliderAttribute.h>
#include <Jimara/Components/Camera.h>
#include <Jimara/Components/Transform.h>
#include <Jimara/Components/Physics/Collider.h>
#include <Jimara/Components/Physics/Rigidbody.h>
#include <Jimara/Components/Animation/Animator.h>
#include <Jimara/Components/Audio/AudioSource.h>
#include <Jimara/Components/GraphicsObjects/ParticleRenderer.h>
#include <Jimara/Core/EnumClassBooleanOperands.h>
#include <Jimara-GenericInputs/Base/VectorInput.h>
#include <type_traits>
#include <utility>


namespace Game {
	namespace Math {
		using namespace ::Jimara::Math;
	}

	namespace OS {
		using namespace ::Jimara::OS;
	}

	using Object = ::Jimara::Object;

	template<typename Type, typename ReferenceCounter = Jimara::JimaraReferenceCounter>
	using Reference = ::Jimara::Reference<Type, ReferenceCounter>;

	template<typename Type>
	using WeakReference = ::Jimara::WeakReference<Type>;

	template<typename... Types>
	using Function = ::Jimara::Function<Types...>;

	template<typename... Types>
	using Callback = ::Jimara::Callback<Types...>;

	template<typename... Types>
	using Event = ::Jimara::Event<Types...>;

	template<typename... Types>
	using InputProvider = ::Jimara::InputProvider<Types...>;

	using Vector2 = ::Jimara::Vector2;
	using Vector3 = ::Jimara::Vector3;
	using Vector4 = ::Jimara::Vector4;
	using Matrix2 = ::Jimara::Matrix2;
	using Matrix3 = ::Jimara::Matrix3;
	using Matrix4 = ::Jimara::Matrix4;

	using Size2 = ::Jimara::Size2;
	using Size3 = ::Jimara::Size3;

	using Component = ::Jimara::Component;
	using UpdatingComponent = ::Jimara::SceneContext::UpdatingComponent;
	using Transform = ::Jimara::Transform;
	using Rigidbody = ::Jimara::Rigidbody;
	using Collider = ::Jimara::Collider;
	using Animator = ::Jimara::Animator;
	using Camera = ::Jimara::Camera;
	using AudioSource = ::Jimara::AudioSource;

	using SerializedObject = ::Jimara::Serialization::SerializedObject;
	using ReportFiedlFn = Callback<SerializedObject>;

	using SerializedCallback = ::Jimara::Serialization::SerializedCallback;

	using SpinLock = ::Jimara::SpinLock;

	template<typename Type>
	using EnumAttribute = ::Jimara::Serialization::EnumAttribute<
		typename std::conditional<std::is_enum_v<Type>, std::underlying_type_t<Type>, Type>::type>;

	template<typename ValueType>
	struct InputWithDefaultValue final {
		ValueType baseValue;
		WeakReference<InputProvider<ValueType>> input;

		inline InputWithDefaultValue(const ValueType& base = {}, InputProvider<ValueType>* inputProvider = nullptr)
			: baseValue(base), input(inputProvider) {}

		inline operator ValueType()const {
			return InputProvider<ValueType>::GetInput(input, baseValue);
		}

		struct Serializer : public virtual ::Jimara::Serialization::SerializerList::From<InputWithDefaultValue> {
			const Reference<const ::Jimara::Serialization::ItemSerializer::Of<ValueType>> baseValueSerializer;
			const Reference<const ::Jimara::Serialization::ItemSerializer::Of<WeakReference<InputProvider<ValueType>>>> inputSerializer;
			inline Serializer(const std::string_view& name, std::string_view& hint, const std::vector<Reference<const Object>>& attributes)
				: ::Jimara::Serialization::ItemSerializer(name, hint, attributes)
				, baseValueSerializer(::Jimara::Serialization::DefaultSerializer<ValueType>
					::Create("Base", "Base value if input is not present", attributes))
				, inputSerializer(::Jimara::Serialization::DefaultSerializer<WeakReference<InputProvider<ValueType>>>
					::Create("Input", "Input provider (if value is present, overrides the base value)", attributes)) {}
			inline virtual ~Serializer() {}
			inline virtual void GetFields(const Callback<SerializedObject>& recordElement, InputWithDefaultValue* target)const override {
				recordElement(baseValueSerializer->Serialize(target->baseValue));
				recordElement(inputSerializer->Serialize(target->input));
			}
		};
	};

	
#define DT_FIELD_TYPE(DT_CLASS, DT_FIELD) std::remove_const_t<std::remove_reference_t< \
		std::conditional_t<std::is_function_v<std::remove_pointer_t<decltype(&DT_CLASS::DT_FIELD)>>, std::invoke_result<std::remove_pointer_t<decltype(&DT_CLASS::DT_FIELD)>>, \
		std::conditional_t<std::is_member_function_pointer_v<decltype(&DT_CLASS::DT_FIELD)>, std::invoke_result<decltype(&DT_CLASS::DT_FIELD), DT_CLASS>, \
		std::remove_pointer<decltype(&(DT_CLASS::DT_FIELD))>>>::type>>

	static_assert(std::is_same_v<DT_FIELD_TYPE(std::vector<int>, size), std::size_t>);
	static_assert(std::is_same_v<DT_FIELD_TYPE(Vector2, x), float>);
	static_assert(std::is_same_v<DT_FIELD_TYPE(std::numeric_limits<float>, epsilon), float>);


#define DT_IMPLEMENT_FIELD_INPUT_COMPONENT(DT_CLASS_NAME, DT_SRC_CLASS, DT_FIELD_NAME) \
	class DT_CLASS_NAME \
		: public virtual Component \
		, public virtual ::Jimara::InputProvider<DT_FIELD_TYPE(DT_SRC_CLASS, DT_FIELD_NAME)> { \
	public: \
		inline DT_CLASS_NAME(::Jimara::Component* parent, const ::std::string_view& name = #DT_CLASS_NAME) : Component(parent, name) {} \
		inline virtual ~DT_CLASS_NAME() {} \
		inline virtual std::optional<DT_FIELD_TYPE(DT_SRC_CLASS, DT_FIELD_NAME)> GetInput()override { \
			::Jimara::Reference<DT_SRC_CLASS> src = m_src; \
			if (src == nullptr) src = GetComponentInParents<::std::remove_const_t<DT_SRC_CLASS>>(); \
			if (src == nullptr) return std::nullopt; \
			return src->DT_FIELD_NAME; \
		} \
		inline virtual void GetFields(::Jimara::Callback<::Jimara::Serialization::SerializedObject> report)override { \
			::Jimara::Component::GetFields(report); \
			static const auto serializer = ::Jimara::Serialization::DefaultSerializer<WeakReference<DT_SRC_CLASS>>::Create("Source", "Input source"); \
			report(serializer->Serialize(m_src)); \
		} \
		inline virtual void FillWeakReferenceHolder(::Jimara::WeaklyReferenceable::WeakReferenceHolder& holder)override { \
			::Jimara::Component::FillWeakReferenceHolder(holder); \
		} \
		inline virtual void ClearWeakReferenceHolder(::Jimara::WeaklyReferenceable::WeakReferenceHolder& holder)override { \
			::Jimara::Component::ClearWeakReferenceHolder(holder); \
		} \
	private: \
		::Jimara::WeakReference<DT_SRC_CLASS> m_src; \
	}


	template<typename Type>
	inline static void ReportTypes(const Callback<::Jimara::TypeId>& report) {
		report(::Jimara::TypeId::Of<Type>());
	}

	template<typename FirstT, typename SecondT, typename... Rest>
	inline static void ReportTypes(const Callback<::Jimara::TypeId>& report) {
		report(::Jimara::TypeId::Of<FirstT>());
		ReportTypes<SecondT, Rest...>(report);
	}

#define DT_EXPOSE_COMPONENT_DETAILS(DT_CLASS_NAME, DT_CLASS_PATH, DT_CLASS_HINT, ...) \
	static_assert(std::is_base_of_v<::Jimara::Component, DT_CLASS_NAME>); \
	namespace Jimara { \
		template<> inline void TypeIdDetails::GetParentTypesOf<DT_CLASS_NAME>(const Callback<TypeId>& report) { \
			::Game::ReportTypes<Component, __VA_ARGS__>(report); \
		} \
		template<> inline static void TypeIdDetails::GetTypeAttributesOf<DT_CLASS_NAME>(const Callback<const Object*>& reportTypeAttributes) { \
			static const ::Jimara::Reference<::Jimara::ComponentFactory> factory = \
				::Jimara::ComponentFactory::Create<DT_CLASS_NAME>(#DT_CLASS_NAME, DT_CLASS_PATH, DT_CLASS_HINT); \
			reportTypeAttributes(factory); \
		} \
	}
}
