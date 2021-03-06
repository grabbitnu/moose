/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "BodyForce.h"

// MOOSE
#include "Function.h"

template<>
InputParameters validParams<BodyForce>()
{
  InputParameters params = validParams<Kernel>();
  params.set<Real>("value")=0.0;
  params.addParam<FunctionName>("function", "", "A function that describes the body force");
  return params;
}

BodyForce::BodyForce(const std::string & name, InputParameters parameters) :
    Kernel(name, parameters),
    _value(getParam<Real>("value")),
    _has_function(getParam<FunctionName>("function") != ""),
    _function( _has_function ? &getFunction("function") : NULL )
{
}

Real
BodyForce::computeQpResidual()
{
  Real factor = _value;
  if (_has_function)
  {
    factor *= _function->value(_t, _q_point[_qp]);
  }
  return _test[_i][_qp]*-factor;
}
