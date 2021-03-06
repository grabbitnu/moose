#include "MatDiffusion.h"


template<>
InputParameters validParams<MatDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addParam<std::string>("D_name","D","The name of the diffusivity");

  return params;
}

MatDiffusion::MatDiffusion(const std::string & name, InputParameters parameters)
  :Diffusion(name, parameters),
   _D_name(getParam<std::string>("D_name")),
   _D(getMaterialProperty<Real>(_D_name))
{}

Real
MatDiffusion::computeQpResidual()
{
  return _D[_qp]*_grad_test[_i][_qp]*_grad_u[_qp];

}

Real
MatDiffusion::computeQpJacobian()
{
  return _D[_qp]*_grad_test[_i][_qp]*_grad_phi[_j][_qp];
}
