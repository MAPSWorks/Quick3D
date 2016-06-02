
#ifndef __VECTOR4_H__
#define __VECTOR4_H__

#include "math.h"
#include "Angles.h"

namespace Math
{
	struct Vector4
	{

	public:

		double X;
		double Y;
		double Z;
		double W;

		//! Constructeur par d�faut
		inline Vector4 ()
		{
			X = 0.0;
			Y = 0.0;
			Z = 0.0;
			W = 0.0;
		}

		//! Constructeur par composants
		inline Vector4 (double NewX, double NewY, double NewZ, double NewW)
		{
			X = NewX;
			Y = NewY;
			Z = NewZ;
			W = NewW;
		}

		//! Constructeur de copie
		inline Vector4 (const Vector4& Target)
		{
			*this = Target;
		}

		//! Renvoie la longueur du vecteur
		inline double getMagnitude () const
		{
			return sqrt(SumComponentSqrs());
		}

		//! Assigne la longueur du vecteur
		inline void setMagnitude (double value)
		{
			if (value >= 0.0)
			{
				if (*this != Vector4(0.0, 0.0, 0.0, 0.0))
				{
					Vector4 Res = (*this) * (value / getMagnitude());

					X = Res.X;
					Y = Res.Y;
					Z = Res.Z;
					W = Res.W;
				}
			}
		}

		//! Renvoie vrai si le vecteur est unitaire
		inline bool IsUnitVector()
		{
			return getMagnitude() == 1.0;
		}

		//! Normalise le vecteur = longueur de 1.0
		inline Vector4 Normalize()
		{
			if (getMagnitude() != 0.0)
			{
				double inverse = 1.0 / getMagnitude();

				return Vector4
				(
					X * inverse,
					Y * inverse,
					Z * inverse,
					W * inverse
				);
			}

			return Vector4();
		}

		//! Renvoie la somme des composants
		inline double SumComponents() const
		{
			return (X + Y + Z + W);
		}

		//! Calcule la racine carr�e des composants
		inline Vector4 SqrtComponents() const
		{
			return Vector4
			(
				sqrt(X),
				sqrt(Y),
				sqrt(Z),
				W
			);
		}

		//! Calcule le carr� des composants
		inline Vector4 SqrComponents () const
		{
			return Vector4
			(
				X * X,
				Y * Y,
				Z * Z,
				W
			);
		}

		//! Calcule la somme des carr�s des composants
		inline static double SumComponentSqrs (const Vector4& v1)
		{
			Vector4 v2 = v1.SqrComponents();

			return v2.SumComponents();
		}

		//! Calcule la somme des carr�s des composants
		inline double SumComponentSqrs() const
		{
			return SumComponentSqrs(*this);
		}

		//! Op�rateur d'�galit�
		inline bool operator == (const Vector4& V2) const
		{
			return (X == V2.X) && (Y == V2.Y) && (Z == V2.Z) && (W == V2.W);
		}

		//! Op�rateur d'in�galit�
		inline bool operator != (const Vector4& V2) const
		{
			return !(*this == V2);
		}

		//! Op�rateur d'assignation
		inline Vector4& operator = (const Vector4& Target)
		{
			X = Target.X;
			Y = Target.Y;
			Z = Target.Z;
			W = Target.W;

			return *this;
		}

		//! Op�rateur d'addition
		inline Vector4 operator + (const Vector4& V2) const
		{
			return Vector4(X + V2.X, Y + V2.Y, Z + V2.Z, W + V2.W);
		}

		//! Op�rateur de soustraction
		inline Vector4 operator - (const Vector4& V2) const
		{
			return Vector4(X - V2.X, Y - V2.Y, Z - V2.Z, W - V2.W);
		}

		//! Op�rateur de multiplication
		inline Vector4 operator * (const Vector4& V2) const
		{
			return Vector4(X * V2.X, Y * V2.Y, Z * V2.Z, W * V2.W);
		}

		//! Op�rateur d'addition
		Vector4 &operator += (const Vector4 &V2)
		{
			X += V2.X;
			Y += V2.Y;
			Z += V2.Z;
			W += V2.W;
			return *this;
		}

		//! Op�rateur de soustraction
		Vector4 &operator -= (const Vector4 &V2)
		{
			X -= V2.X;
			Y -= V2.Y;
			Z -= V2.Z;
			W -= V2.W;
			return *this;
		}

		//! Op�rateur de multiplication avec un double
		inline Vector4 operator * (double s2) const
		{
			return Vector4
			(
				X * s2,
				Y * s2,
				Z * s2,
				W * s2
			);
		}

		//! Op�rateur de division avec un double
		inline Vector4 operator / (double s2) const
		{
			return Vector4
			(
				X / s2,
				Y / s2,
				Z / s2,
				W / s2
			);
		}

	    inline double &operator[](unsigned int index)
		{
			return (&X)[index];
		}

		inline const double &operator[](unsigned int index) const
		{
			return (&X)[index];
		}

		//! Produit vectoriel
		inline Vector4 CrossProduct(Vector4& v2) const
		{
			return Vector4
			(
				Y * v2.Z - Z * v2.Y,
				Z * v2.X - X * v2.Z,
				X * v2.Y - Y * v2.X,
				W
			);
		}

		//! Produit scalaire
		inline double DotProduct(const Vector4& v2) const
		{
			return
			(
				X * v2.X +
				Y * v2.Y +
				Z * v2.Z
			);
		}

		// Renvoie l'angle sur l'axe des X
		inline double AngleX() const
		{
			if (Y == 0.0) return 0.0;
			return atan2(Z, Y) - (Math::Pi / 2.0);
		}

		// Renvoie l'angle sur l'axe des Y
		inline double AngleY() const
		{
			if (Z == 0.0)
			{
				if (X == 0.0) return 0.0;
				return atan2(Z, X) + (Math::Pi / 2.0);
			}
			return atan2(X, Z);
		}

		// Renvoie l'angle sur l'axe des Z
		inline double AngleZ() const
		{
			if (X == 0.0) return 0.0;
			return atan2(Y, X);
		}
	};
}

#endif // __VECTOR4_H__