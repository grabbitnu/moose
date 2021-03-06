#include "NSWeakStagnationBC.h"

// Full specialization of the validParams function for this object
template<>
InputParameters validParams<NSWeakStagnationBC>()
{
  // Initialize the params object from the base class
  InputParameters params = validParams<NSIntegratedBC>();

  // Required parameters
  params.addRequiredParam<Real>("stagnation_pressure", "The specifed stagnation pressure");
  params.addRequiredParam<Real>("stagnation_temperature", "The specifed stagnation temperature");

  params.addRequiredParam<Real>("sx", "x-component of specifed flow direction");
  params.addRequiredParam<Real>("sy", "y-component of specifed flow direction");
  params.addParam<Real>("sz", 0., "z-component of specifed flow direction"); // only required in 3D

  return params;
}




// Constructor, be sure to call the base class constructor first!
NSWeakStagnationBC::NSWeakStagnationBC(const std::string & name, InputParameters parameters)
    : NSIntegratedBC(name, parameters),

      // Required parameters
      _stagnation_pressure(getParam<Real>("stagnation_pressure")),
      _stagnation_temperature(getParam<Real>("stagnation_temperature")),
      _sx(getParam<Real>("sx")),
      _sy(getParam<Real>("sy")),
      _sz(getParam<Real>("sz"))
{}




void NSWeakStagnationBC::static_values(Real& T_s, Real& p_s, Real& rho_s)
{
  // T_s = T_0 - |u|^2/2/cp
  Real cv = _R / (_gamma-1.);
  T_s = _stagnation_temperature - 0.5 * this->velmag2() / (_gamma * cv);

  if (T_s < 0.)
    mooseError("Negative temperature detected in NSWeakStagnationBC!");

  // p_s = p_0 * (T_0/T)^(-gam/(gam-1))
  p_s = _stagnation_pressure * std::pow(_stagnation_temperature/T_s, -_gamma / (_gamma-1.));

  // rho_s = p_s / R / T_s
  rho_s = p_s / _R / T_s;
}




Real NSWeakStagnationBC::rho_static()
{
  Real T_s = 0., p_s = 0., rho_s = 0.;
  this->static_values(T_s, p_s, rho_s);
  return rho_s;
}



Real NSWeakStagnationBC::velmag2()
{
  return
    _u_vel[_qp]*_u_vel[_qp] +
    _v_vel[_qp]*_v_vel[_qp] +
    _w_vel[_qp]*_w_vel[_qp];
}




Real NSWeakStagnationBC::sdotn()
{
  return
    _sx*_normals[_qp](0) +
    _sy*_normals[_qp](1) +
    _sz*_normals[_qp](2);
}
