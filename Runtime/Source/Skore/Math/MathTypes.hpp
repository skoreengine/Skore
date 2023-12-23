// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Base.hpp"
namespace Skore
{

	struct Extent
	{
		u32 width;
		u32 height;
	};

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

	template<typename Type>
	struct TVec3
	{

		union
		{
			struct
			{
				Type x;
				Type y;
				Type z;
			};
			Type coord[3] = {0};
		};

		inline const Type& operator[](int axis) const
		{
			return coord[axis];
		}

		inline Type& operator[](int axis)
		{
			return coord[axis];
		}

		inline TVec3 operator/(const TVec3& b) const
		{
			return {this->x / b.x, this->y / b.y, this->z / b.z};
		}

		inline TVec3 operator*(const TVec3& b) const
		{
			return {this->x * b.x, this->y * b.y, this->z * b.z};
		}

		inline TVec3 operator-(const TVec3& b) const
		{
			return {this->x - b.x, this->y - b.y, this->z - b.z};
		}

		inline TVec3 operator/(const Type& b) const
		{
			return {this->x / b, this->y / b, this->z / b};
		}

		inline TVec3 operator+(const Type& b) const
		{
			return {this->x + b, this->y + b, this->z + b};
		}

		inline TVec3 operator-(const Type& b) const
		{
			return {this->x - b, this->y - b, this->z - b};
		}

		inline TVec3 operator*(const Type& b) const
		{
			return {this->x * b, this->y * b, this->z * b};
		}

		inline TVec3 operator>>(const int vl) const
		{
			return {this->x >> vl, this->y >> vl, this->z >> vl};
		}

		inline TVec3 operator<<(const int vl) const
		{
			return {this->x << vl, this->y << vl, this->z << vl};
		}

		inline TVec3& operator*=(const TVec3& rhs)
		{
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
			return *this;
		}

		inline TVec3& operator+=(const TVec3& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			return *this;
		}

		inline TVec3& operator-=(const TVec3& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			return *this;
		}

		template<typename U>
		inline TVec3 operator/=(U u)
		{
			this->x /= static_cast<Type>(u);
			this->y /= static_cast<Type>(u);
			this->z /= static_cast<Type>(u);
			return *this;
		}

		static const TVec3& Zero()
		{
			static const auto zero = TVec3{0, 0, 0};
			return zero;
		}

		inline const Type* operator[](usize axis) const
		{
			return coord[axis];
		}

		inline Type* operator[](usize axis)
		{
			return coord[axis];
		}

		TVec3<Type> operator-()
		{
			TVec3<Type> ret{};
			ret.x = this->x * -1;
			ret.y = this->y * -1;
			ret.z = this->z * -1;
			return ret;
		}

		bool operator==(const Type& b) const;

		bool operator==(const TVec3<Type>& b) const;

		bool operator>(const Type& b) const;

		static inline TVec3 AxisX()
		{
			return {static_cast<Type>(1), static_cast<Type>(0), static_cast<Type>(0)};
		}

		static inline TVec3 AxisY()
		{
			return {static_cast<Type>(0), static_cast<Type>(1), static_cast<Type>(0)};
		}

		static inline TVec3 AxisZ()
		{
			return {static_cast<Type>(0), static_cast<Type>(0), static_cast<Type>(1)};
		}
	};

	template<typename Type>
	struct TVec4
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

				union
				{
					Type z, b;
				};

				union
				{
					Type w, a;
				};

			};
			Type c[4] = {0};
		};

		TVec4() = default;

		TVec4(const Type value)
		{
			c[0] = value;
			c[1] = value;
			c[2] = value;
			c[3] = value;
		}

		TVec4(const Type _x, const Type _y, const Type _z, const Type _w)
		{
			c[0] = _x;
			c[1] = _y;
			c[2] = _z;
			c[3] = _w;
		}

		TVec4(const TVec3 <Type>& vec, const Type _w)
		{
			c[0] = vec.x;
			c[1] = vec.y;
			c[2] = vec.z;
			c[3] = _w;
		}

		inline const Type& operator[](int axis) const
		{
			return c[axis];
		}

		inline Type& operator[](int axis)
		{
			return c[axis];
		}

		inline TVec4 operator/(const TVec4& b) const
		{
			return {this->x / b.x, this->y / b.y, this->z / b.z, this->w / b.w};
		}

		inline TVec4 operator+(const TVec4& b) const
		{
			return {this->x + b.x, this->y + b.y, this->z + b.z, this->w + b.w};
		}

		inline TVec4 operator*(const TVec4& b) const
		{
			return {this->x * b.x, this->y * b.y, this->z * b.z, this->w * b.w};
		}

		inline TVec4 operator-(const TVec4& b) const
		{
			return {this->x - b.x, this->y - b.y, this->z - b.z, this->w - b.w};
		}

		inline TVec4 operator/(const Type& b) const
		{
			return {this->x / b, this->y / b, this->z / b, this->w / b};
		}

		inline TVec4 operator*(const Type& b) const
		{
			return {this->x * b, this->y * b, this->z * b, this->w * b};
		}

		inline TVec4 operator>>(const int vl) const
		{
			return {this->x >> vl, this->y >> vl, this->z >> vl, this->w >> vl};
		}

		inline TVec4 operator<<(const int vl) const
		{
			return {this->x << vl, this->y << vl, this->z << vl, this->w << vl};
		}

		bool operator==(const Type& b) const;

		bool operator==(const TVec4<Type>& b) const;

		bool operator!=(const Type& b) const;

		bool operator!=(const TVec4<Type>& b) const;

	};


	template<typename Type>
	bool TVec3<Type>::operator==(const Type& b) const
	{
		return Compare<Type>::IsEqual(x, b) && Compare<Type>::IsEqual(y, b) && Compare<Type>::IsEqual(z, b);
	}

	template<typename Type>
	bool TVec3<Type>::operator==(const TVec3<Type>& b) const
	{
		return Compare<Type>::IsEqual(x, b.x) && Compare<Type>::IsEqual(y, b.y) && Compare<Type>::IsEqual(z, b.z);
	}

	template<typename Type>
	bool TVec3<Type>::operator>(const Type& b) const
	{
		return x > b && y > b && z > b;
	}

	template<typename T>
	inline TVec3<T> operator-(const TVec3<T>& a, const TVec3<T>& b)
	{
		return {a.x - b.x, a.y - b.y, a.z - b.z};
	}

	template<typename T>
	inline TVec3<T> operator+(const TVec3<T>& a, const TVec3<T>& b)
	{
		return {a.x + b.x, a.y + b.y, a.z + b.z};
	}

	template<typename T>
	inline TVec3<T> operator*(const T& a, const TVec3<T>& b)
	{
		return {a * b.x, a * b.y, a * b.z};
	}

	template<typename T>
	inline TVec3<T> operator/(const T& a, const TVec3<T>& b)
	{
		return {a / b.x, a / b.y, a / b.z};
	}

	template<typename T>
	inline TVec3<T> zero(const T& a, const TVec3<T>& b)
	{
		return {a * b.x, a * b.y, a * b.z};
	}

	template<typename Type>
	bool TVec4<Type>::operator==(const Type& b) const
	{
		return Compare<Type>::IsEqual(x, b) && Compare<Type>::IsEqual(y, b) && Compare<Type>::IsEqual(z, b) && Compare<Type>::IsEqual(w, b);
	}

	template<typename Type>
	bool TVec4<Type>::operator==(const TVec4<Type>& b) const
	{
		return Compare<Type>::IsEqual(x, b.x) && Compare<Type>::IsEqual(y, b.y) && Compare<Type>::IsEqual(z, b.z) && Compare<Type>::IsEqual(w, b.w);
	}

	template<typename Type>
	bool TVec4<Type>::operator!=(const Type& b) const
	{
		return !(*this == b);
	}

	template<typename Type>
	bool TVec4<Type>::operator!=(const TVec4<Type>& b) const
	{
		return !(*this == b);
	}

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

	using Vec3 = TVec3<f32>;
	using IVec3 = TVec3<i32>;
	using UVec3 = TVec3<u32>;

	using Vec4 = TVec4<f32>;
	using IVec4 = TVec4<i32>;
	using UVec4 = TVec4<u32>;
}