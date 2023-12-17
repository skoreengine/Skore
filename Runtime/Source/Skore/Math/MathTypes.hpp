// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
namespace Skore
{
	struct Rect
	{
		i32 x;
		i32 y;
		u32 width;
		u32 height;
	};

	struct FRect
	{
		f32 x;
		f32 y;
		f32 width;
		f32 height;
	};

	template<typename Type>
	struct TVec2
	{
		union
		{
			struct
			{
				union
				{
					Type x, r, width;
				};

				union
				{
					Type y, g, height;
				};
			};
			Type c[2] = {static_cast<Type>(0)};
		};

		TVec2 operator/(const TVec2& b) const
		{
			return {this->x / b.x, this->y / b.y};
		}

		TVec2 operator/(const Type& b) const
		{
			return {this->x / b, this->y / b};
		}

		TVec2 operator*(const Type& b) const
		{
			return TVec2{this->x * b, this->y * b};
		}

		TVec2 operator*(const TVec2<Type>& b) const
		{
			return TVec2{this->x * b.x, this->y * b.y};
		}

		TVec2 operator+(const TVec2<Type>& b) const
		{
			return TVec2{this->x + b.x, this->y + b.y};
		}

		TVec2 operator-(const TVec2<Type>& b) const
		{
			return TVec2{this->x - b.x, this->y - b.y};
		}

		TVec2 operator>>(const int vl) const
		{
			return TVec2{this->x >> vl, this->y >> vl};
		}

		TVec2 operator<<(const int vl) const
		{
			return TVec2{this->x << vl, this->y << vl};
		}

		bool operator==(const Type& b) const
		{
			return this->x == b && this->y == b;
		}

		bool operator!=(const Type& b) const
		{
			return !(*this == b);
		}

		inline const Type& operator[](usize axis) const
		{
			return c[axis];
		}

		inline Type& operator[](usize axis)
		{
			return c[axis];
		}

		inline TVec2& operator*=(const TVec2& rhs)
		{
			this->x *= rhs.x;
			this->y *= rhs.y;
			return *this;
		}

		template<typename T>
		inline TVec2& operator*=(const T& rhs)
		{
			this->x *= rhs;
			this->y *= rhs;
			return *this;
		}

		inline bool operator>(const Type& b) const
		{
			return x > b && y > b;
		}

	};

	template<typename T>
	inline bool operator==(const TVec2<T>& a, const TVec2<T>& b)
	{
		return a.x == b.x && a.y == b.y;
	}

	template<typename T>
	inline bool operator!=(const TVec2<T>& a, const TVec2<T>& b)
	{
		return !(a == b);
	}

	template<typename Type, typename Type2>
	TVec2<Type> operator*(const TVec2<Type>& a, const TVec2<Type2>& b)
	{
		return {a.x * b.x, a.y * b.y};
	}


	template<typename T>
	inline TVec2<T> MakeVec2(const T* value)
	{
		return TVec2<T>{value[0], value[1]};
	}


	using Vec2 = TVec2<f32>;
	using IVec2 = TVec2<i32>;
	using UVec2 = TVec2<u32>;
}