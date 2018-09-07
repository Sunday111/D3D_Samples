#include <memory>
#include "Keng/Reflection/Detail/MakeTypeOperators.h"


namespace keng::reflection::detail {
	void ValidateOperatorsGeneration() {
		{
			struct T0
			{
				bool operator==(const T0&) const { return true; }
			};

			constexpr auto operators = MakeTypeOperators<T0>();
			static_assert(operators.defaultConstructor);
			static_assert(operators.copyConstructor);
			static_assert(operators.moveConstructor);
			static_assert(operators.copyAssign);
			static_assert(operators.moveAssign);
			static_assert(operators.destructor);
			static_assert(operators.equals);
		}

		{
			struct T1
			{
				T1(const T1&) = delete;
			};

			constexpr auto operators = MakeTypeOperators<T1>();
			static_assert(!operators.defaultConstructor);
			static_assert(!operators.copyConstructor);
			static_assert(!operators.moveConstructor);
			static_assert(operators.copyAssign);
			static_assert(operators.moveAssign);
			static_assert(operators.destructor);
			static_assert(!operators.equals);
		}

		{
			struct T2
			{
				T2(const T2&) = delete;
				T2& operator=(const T2&) = delete;
			};

			constexpr auto operators = MakeTypeOperators<T2>();
			static_assert(!operators.defaultConstructor);
			static_assert(!operators.copyConstructor);
			static_assert(!operators.moveConstructor);
			static_assert(!operators.copyAssign);
			static_assert(!operators.moveAssign);
			static_assert(operators.destructor);
			static_assert(!operators.equals);
		}

		{
			struct T3
			{
				T3() = default;
				T3(const T3&) = delete;
				T3& operator=(const T3&) = delete;
			};

			constexpr auto operators = MakeTypeOperators<T3>();
			static_assert(operators.defaultConstructor);
			static_assert(!operators.copyConstructor);
			static_assert(!operators.moveConstructor);
			static_assert(!operators.copyAssign);
			static_assert(!operators.moveAssign);
			static_assert(operators.destructor);
			static_assert(!operators.equals);
		}

		{
			struct T4
			{
				T4() = default;
				T4(T4&&) = default;
				T4(const T4&) = delete;
				T4& operator=(const T4&) = delete;
			};

			constexpr auto operators = MakeTypeOperators<T4>();
			static_assert(operators.defaultConstructor);
			static_assert(!operators.copyConstructor);
			static_assert(operators.moveConstructor);
			static_assert(!operators.copyAssign);
			static_assert(!operators.moveAssign);
			static_assert(operators.destructor);
			static_assert(!operators.equals);
		}

		{
			struct T5
			{
				T5() = default;
				T5(T5&&) = default;
				T5(const T5&) = delete;
				T5& operator=(const T5&) = delete;
				T5& operator=(T5&&) = default;
			};

			constexpr auto operators = MakeTypeOperators<T5>();
			static_assert(operators.defaultConstructor);
			static_assert(!operators.copyConstructor);
			static_assert(operators.moveConstructor);
			static_assert(!operators.copyAssign);
			static_assert(operators.moveAssign);
			static_assert(operators.destructor);
			static_assert(!operators.equals);
		}
	}
}
