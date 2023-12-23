// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Base.hpp"
namespace Skore
{

	struct Extent
	{
		u32 Width;
		u32 Height;
	};

	struct Rect
	{
		i32 X;
		i32 Y;
		u32 Width;
		u32 Height;
	};

	struct FRect
	{
		f32 X;
		f32 Y;
		f32 Width;
		f32 Height;
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
					Type X, R, Width;
				};

				union
				{
					Type Y, G, Height;
				};
			};
			Type Coord[2] = {static_cast<Type>(0)};
		};

		TVec2 operator/(const TVec2& b) const
		{
			return {this->X / b.X, this->Y / b.Y};
		}

		TVec2 operator/(const Type& b) const
		{
			return {this->X / b, this->Y / b};
		}

		TVec2 operator*(const Type& b) const
		{
			return TVec2{this->X * b, this->Y * b};
		}

		TVec2 operator*(const TVec2<Type>& b) const
		{
			return TVec2{this->X * b.X, this->Y * b.Y};
		}

		TVec2 operator+(const TVec2<Type>& b) const
		{
			return TVec2{this->X + b.X, this->Y + b.Y};
		}

		TVec2 operator-(const TVec2<Type>& b) const
		{
			return TVec2{this->X - b.X, this->Y - b.Y};
		}

		TVec2 operator>>(const int vl) const
		{
			return TVec2{this->X >> vl, this->Y >> vl};
		}

		TVec2 operator<<(const int vl) const
		{
			return TVec2{this->X << vl, this->Y << vl};
		}

		bool operator==(const Type& b) const
		{
			return this->X == b && this->Y == b;
		}

		bool operator!=(const Type& b) const
		{
			return !(*this == b);
		}

		inline const Type& operator[](usize axis) const
		{
			return Coord[axis];
		}

		inline Type& operator[](usize axis)
		{
			return Coord[axis];
		}

		inline TVec2& operator*=(const TVec2& rhs)
		{
			this->X *= rhs.X;
			this->Y *= rhs.Y;
			return *this;
		}

		template<typename T>
		inline TVec2& operator*=(const T& rhs)
		{
			this->X *= rhs;
			this->Y *= rhs;
			return *this;
		}

		inline bool operator>(const Type& b) const
		{
			return X > b && Y > b;
		}
	};

	template<typename Type>
	struct TVec3
	{

		union
		{
			struct
			{
				Type X;
				Type Y;
				Type Z;
			};
			Type Coord[3] = {0};
		};

		inline const Type& operator[](int axis) const
		{
			return Coord[axis];
		}

		inline Type& operator[](int axis)
		{
			return Coord[axis];
		}

		inline TVec3 operator/(const TVec3& b) const
		{
			return {this->X / b.X, this->Y / b.Y, this->Z / b.Z};
		}

		inline TVec3 operator*(const TVec3& b) const
		{
			return {this->X * b.X, this->Y * b.Y, this->Z * b.Z};
		}

		inline TVec3 operator-(const TVec3& b) const
		{
			return {this->X - b.X, this->Y - b.Y, this->Z - b.Z};
		}

		inline TVec3 operator/(const Type& b) const
		{
			return {this->X / b, this->Y / b, this->Z / b};
		}

		inline TVec3 operator+(const Type& b) const
		{
			return {this->X + b, this->Y + b, this->Z + b};
		}

		inline TVec3 operator-(const Type& b) const
		{
			return {this->X - b, this->Y - b, this->Z - b};
		}

		inline TVec3 operator*(const Type& b) const
		{
			return {this->X * b, this->Y * b, this->Z * b};
		}

		inline TVec3 operator>>(const int vl) const
		{
			return {this->X >> vl, this->Y >> vl, this->Z >> vl};
		}

		inline TVec3 operator<<(const int vl) const
		{
			return {this->X << vl, this->Y << vl, this->Z << vl};
		}

		inline TVec3& operator*=(const TVec3& rhs)
		{
			this->X *= rhs.X;
			this->Y *= rhs.Y;
			this->Z *= rhs.Z;
			return *this;
		}

		inline TVec3& operator+=(const TVec3& rhs)
		{
			this->X += rhs.X;
			this->Y += rhs.Y;
			this->Z += rhs.Z;
			return *this;
		}

		inline TVec3& operator-=(const TVec3& rhs)
		{
			this->X -= rhs.X;
			this->Y -= rhs.Y;
			this->Z -= rhs.Z;
			return *this;
		}

		template<typename U>
		inline TVec3 operator/=(U u)
		{
			this->X /= static_cast<Type>(u);
			this->Y /= static_cast<Type>(u);
			this->Z /= static_cast<Type>(u);
			return *this;
		}

		static const TVec3& Zero()
		{
			static const auto zero = TVec3{0, 0, 0};
			return zero;
		}

		inline const Type* operator[](usize axis) const
		{
			return Coord[axis];
		}

		inline Type* operator[](usize axis)
		{
			return Coord[axis];
		}

		TVec3<Type> operator-()
		{
			TVec3<Type> ret{};
			ret.X = this->X * -1;
			ret.Y = this->Y * -1;
			ret.Z = this->Z * -1;
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
					Type X, R, Width;
				};

				union
				{
					Type Y, G, Height;
				};

				union
				{
					Type Z, B;
				};

				union
				{
					Type W, A;
				};

			};
			Type Coord[4] = {0};
		};

		TVec4() = default;

		TVec4(const Type value)
		{
			Coord[0] = value;
			Coord[1] = value;
			Coord[2] = value;
			Coord[3] = value;
		}

		TVec4(const Type x, const Type y, const Type z, const Type w)
		{
			Coord[0] = x;
			Coord[1] = y;
			Coord[2] = z;
			Coord[3] = w;
		}

		TVec4(const TVec3 <Type>& vec, const Type w)
		{
			Coord[0] = vec.X;
			Coord[1] = vec.Y;
			Coord[2] = vec.Z;
			Coord[3] = w;
		}

		inline const Type& operator[](i32 axis) const
		{
			return Coord[axis];
		}

		inline Type& operator[](i32 axis)
		{
			return Coord[axis];
		}

		inline TVec4 operator/(const TVec4& b) const
		{
			return {this->X / b.X, this->Y / b.Y, this->Z / b.Z, this->W / b.W};
		}

		inline TVec4 operator+(const TVec4& b) const
		{
			return {this->X + b.X, this->Y + b.Y, this->Z + b.Z, this->W + b.W};
		}

		inline TVec4 operator*(const TVec4& b) const
		{
			return {this->X * b.X, this->Y * b.Y, this->Z * b.Z, this->W * b.W};
		}

		inline TVec4 operator-(const TVec4& b) const
		{
			return {this->X - b.X, this->Y - b.Y, this->Z - b.Z, this->W - b.W};
		}

		inline TVec4 operator/(const Type& b) const
		{
			return {this->X / b, this->Y / b, this->Z / b, this->W / b};
		}

		inline TVec4 operator*(const Type& b) const
		{
			return {this->X * b, this->Y * b, this->Z * b, this->W * b};
		}

		inline TVec4 operator>>(const int vl) const
		{
			return {this->X >> vl, this->Y >> vl, this->Z >> vl, this->W >> vl};
		}

		inline TVec4 operator<<(const int vl) const
		{
			return {this->X << vl, this->Y << vl, this->Z << vl, this->W << vl};
		}

		bool operator==(const Type& b) const;

		bool operator==(const TVec4<Type>& b) const;

		bool operator!=(const Type& b) const;

		bool operator!=(const TVec4<Type>& b) const;

	};


	template<typename Type>
	bool TVec3<Type>::operator==(const Type& b) const
	{
		return Compare<Type>::IsEqual(X, b) && Compare<Type>::IsEqual(Y, b) && Compare<Type>::IsEqual(Z, b);
	}

	template<typename Type>
	bool TVec3<Type>::operator==(const TVec3<Type>& b) const
	{
		return Compare<Type>::IsEqual(X, b.X) && Compare<Type>::IsEqual(Y, b.Y) && Compare<Type>::IsEqual(Z, b.Z);
	}

	template<typename Type>
	bool TVec3<Type>::operator>(const Type& b) const
	{
		return X > b && Y > b && Z > b;
	}

	template<typename T>
	inline TVec3<T> operator-(const TVec3<T>& a, const TVec3<T>& b)
	{
		return {a.X - b.X, a.Y - b.Y, a.Z - b.Z};
	}

	template<typename T>
	inline TVec3<T> operator+(const TVec3<T>& a, const TVec3<T>& b)
	{
		return {a.X + b.X, a.Y + b.Y, a.Z + b.Z};
	}

	template<typename T>
	inline TVec3<T> operator*(const T& a, const TVec3<T>& b)
	{
		return {a * b.X, a * b.Y, a * b.Z};
	}

	template<typename T>
	inline TVec3<T> operator/(const T& a, const TVec3<T>& b)
	{
		return {a / b.X, a / b.Y, a / b.Z};
	}

	template<typename T>
	inline TVec3<T> zero(const T& a, const TVec3<T>& b)
	{
		return {a * b.X, a * b.Y, a * b.Z};
	}

	template<typename Type>
	bool TVec4<Type>::operator==(const Type& b) const
	{
		return Compare<Type>::IsEqual(X, b) && Compare<Type>::IsEqual(Y, b) && Compare<Type>::IsEqual(Z, b) && Compare<Type>::IsEqual(W, b);
	}

	template<typename Type>
	bool TVec4<Type>::operator==(const TVec4<Type>& b) const
	{
		return Compare<Type>::IsEqual(X, b.X) && Compare<Type>::IsEqual(Y, b.Y) && Compare<Type>::IsEqual(Z, b.Z) && Compare<Type>::IsEqual(W, b.W);
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
		return a.X == b.X && a.Y == b.Y;
	}

	template<typename T>
	inline bool operator!=(const TVec2<T>& a, const TVec2<T>& b)
	{
		return !(a == b);
	}

	template<typename Type, typename Type2>
	TVec2<Type> operator*(const TVec2<Type>& a, const TVec2<Type2>& b)
	{
		return {a.X * b.X, a.Y * b.Y};
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