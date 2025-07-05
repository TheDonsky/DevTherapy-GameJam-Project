#pragma once
#include <Jimara/Environment/Scene/Scene.h>
#include <Jimara/Data/Serialization/DefaultSerializer.h>
#include <Jimara/Data/Serialization/Helpers/SerializerMacros.h>
#include <Jimara/Components/Transform.h>
#include <Jimara-GenericInputs/Base/VectorInput.h>


namespace Game {
	namespace Math {
		using namespace Jimara::Math;
	}

	using Object = Jimara::Object;

	using Component = Jimara::Component;
	using Transform = Jimara::Transform;

	template<typename Type, typename ReferenceCounter = Jimara::JimaraReferenceCounter>
	using Reference = Jimara::Reference<Type, ReferenceCounter>;

	template<typename Type>
	using WeakReference = Jimara::WeakReference<Type>;

	template<typename... Types>
	using Function = Jimara::Function<Types...>;

	template<typename... Types>
	using Callback = Jimara::Callback<Types...>;

	template<typename... Types>
	using Event = Jimara::Event<Types...>;

	using Vector2 = Jimara::Vector2;
	using Vector3 = Jimara::Vector3;
	using Vector4 = Jimara::Vector4;
	using Matrix2 = Jimara::Matrix2;
	using Matrix3 = Jimara::Matrix3;
	using Matrix4 = Jimara::Matrix4;
}
