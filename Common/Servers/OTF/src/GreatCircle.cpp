//$Id: GreatCircle.cpp,v 1.2 2008-06-05 07:16:52 s.righini Exp $ 

#include "GreatCircle.h"
#include <math.h>
#include <slalib.h>

GreatCircle::GreatCircle() {}
GreatCircle::~GreatCircle() {}

void GreatCircle::setGreatCircle(const Tspherical& sph_start, const Tspherical& sph_stop) {
	m_sph_start=sph_start;
	m_sph_stop=sph_stop;
	sph2Cart(m_sph_start, m_cart_start);
	sph2Cart(m_sph_stop, m_cart_stop);
	m_psi=acos(m_cart_start.x*m_cart_stop.x+m_cart_start.y*m_cart_stop.y+m_cart_start.z*m_cart_stop.z);
    psi=m_psi;
}

const GreatCircle::Tspherical& GreatCircle::getGreatCircleCenter() {
	return getGreatCirclePoint(m_psi/2.0);
}

const GreatCircle::Tspherical& GreatCircle::getGreatCirclePoint(const double& phi) {
	 Tcartesian cart_point;
	 cart_point.x=(m_cart_start.x*sin(m_psi-phi)+m_cart_stop.x*sin(phi))/sin(m_psi);
	 cart_point.y=(m_cart_start.y*sin(m_psi-phi)+m_cart_stop.y*sin(phi))/sin(m_psi);
	 cart_point.z=(m_cart_start.z*sin(m_psi-phi)+m_cart_stop.z*sin(phi))/sin(m_psi);
	 cart2Sph(cart_point, m_sph_point);
	 return m_sph_point;
}

void GreatCircle::sph2Cart(const Tspherical& sph, Tcartesian& cart){
	cart.x=cos(sph.l)*cos(sph.b);
	cart.y=sin(sph.l)*cos(sph.b);
	cart.z=sin(sph.b);
}

void GreatCircle::cart2Sph(const Tcartesian& cart, Tspherical& sph){
	sph.b=slaDrange(asin(cart.z));
	sph.l=slaDranrm(atan2(cart.y,cart.x));
}


