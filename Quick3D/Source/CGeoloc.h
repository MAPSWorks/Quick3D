
#ifndef __GEOLOC_H__
#define __GEOLOC_H__

#include "quick3d_global.h"

// Std
#include "math.h"

// Qt
#include <QVariant>
#include <QDataStream>
#include <QString>

// Application
#include "CVector3.h"
#include "CAxis.h"

namespace Math
{
	class QUICK3D_EXPORT CGeoloc
	{
	public:

		enum EGeolocDatum
		{
			gld_None,
			gld_Sphere,
			gld_UTM,
			gld_WGS84
		};

		double			Latitude;
		double			Longitude;
		double			Altitude;
		EGeolocDatum	Datum;

		//! Constructeur par d�faut
		CGeoloc ();

		//! Constructeur par composants
		CGeoloc (double NewLatitude, double NewLongitude, double NewAltitude, EGeolocDatum NewDatum = gld_WGS84);

		//! Constructeur de copie
		CGeoloc (const CGeoloc& Target);

		//! Constructeur par vecteur
		//! Le vecteur repr�sente une position g�ocentrique ECEF (Earth-centered Earth-Fixed)
		CGeoloc (const CVector3& vPosition);

		//! Constructeur par vecteur
		//! gReference repr�sente une g�olocalisation de r�f�rence et vPosition une position xyz relative
		//! Le rep�re de vPosition est l'axe NOLL (North-Oriented Local-level) au niveau de gReference
		CGeoloc (const CGeoloc& gReference, const CVector3& vPosition);

		//! Destructeur
		virtual ~CGeoloc ();

		//! Op�rateur de copie
		CGeoloc& operator = (const CGeoloc& Target);

		//! Op�rateur d'�galit�
		bool operator == (const CGeoloc& Target);

		//! Transforme la g�olocalisation dans le rep�re NOLL de gReference
		CVector3 toVector3(const CGeoloc& gReference) const;

		//! Transforme la g�olocalisation en coordonn�e g�ocentrique ECEF (Earth-centered Earth-Fixed)
		CVector3 toVector3() const;

		//! Retourne le rep�re NOLL de cette g�olocalisation (North-Oriented Local-level)
		//! La composante front pointe vers le nord g�ographique
		//! La composante up pointe � l'oppos� de la gravit�
		//! La composante right est perpendiculaire aux deux autres axes
		CAxis getNOLLAxis() const;

		//! Converti un cap magn�tique en cap vrai depuis cette g�olocalisation
		double magneticHeadingtoTrueHeading(double dHeading) const;

		//! Converti un cap vrai en cap magn�tique depuis cette g�olocalisation
		double trueHeadingToMagneticHeading(double dHeading) const;

		//! Retourne vrai si l'objet n'est pas � Lat=0, Lon=0
		bool valid() const { return !(Latitude == 0.0 && Longitude == 0.0); }

		//! Transforme en chaine de caract�res
		virtual QString toString() const;

		//!
		double getPlanetRadius();

	protected:

		//! Transforme la g�olocalisation en coordonn�e g�ocentrique ECEF (Earth-centered Earth-Fixed) via un mod�le sph�rique
		CVector3 toVector3_Sphere() const;

		//! Transforme la g�olocalisation dans le rep�re cart�sien de gReference via un mod�le sph�rique
		CVector3 toVector3_Sphere(const CGeoloc& gReference) const;

		//! Transforme la g�olocalisation en coordonn�e g�ocentrique ECEF (Earth-centered Earth-Fixed) via le mod�le UTM
		CVector3 toVector3_UTM() const;

		//! Transforme la g�olocalisation dans le rep�re cart�sien de gReference via le mod�le UTM
		CVector3 toVector3_UTM(const CGeoloc& gReference) const;

		//! Transforme la g�olocalisation en coordonn�e g�ocentrique ECEF (Earth-centered Earth-Fixed) via le mod�le WGS84
		CVector3 toVector3_WGS84() const;

		//! Transforme la g�olocalisation dans le rep�re cart�sien de gReference via le mod�le WGS84
		CVector3 toVector3_WGS84(const CGeoloc& gReference) const;

		//! Cr�e l'objet depuis une position ECEF (Earth-centered Earth-Fixed) via le mod�le WGS84
		static CGeoloc fromVector3_WGS84(const CVector3& vPosition3D);

		//! Cr�e l'objet depuis une position dans le rep�re cart�sien gReference via le mod�le WGS84
		static CGeoloc fromVector3_WGS84(const CGeoloc& gReference, const CVector3& vPosition3D);

		//! Calcule la d�clinaison magn�tique pour cette g�olocalisation
		double computeMagneticDeclination() const;

		friend QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
		friend QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);
	};

	QUICK3D_EXPORT QDataStream& operator << (QDataStream& out, const CGeoloc&);
	QUICK3D_EXPORT QDataStream& operator >> (QDataStream& in, CGeoloc&);
}

Q_DECLARE_METATYPE(Math::CGeoloc)

#endif // __GEOLOC_H__