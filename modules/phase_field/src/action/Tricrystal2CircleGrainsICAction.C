#include "Tricrystal2CircleGrainsICAction.h"
#include "Factory.h"
#include "Parser.h"
#include "FEProblem.h"

#include <sstream>
#include <stdexcept>

// libMesh includes
#include "libmesh/libmesh.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/equation_systems.h"
#include "libmesh/nonlinear_implicit_system.h"
#include "libmesh/explicit_system.h"
#include "libmesh/string_to_enum.h"

const Real Tricrystal2CircleGrainsICAction::_abs_zero_tol = 1e-12;

template<>
InputParameters validParams<Tricrystal2CircleGrainsICAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<unsigned int>("crys_num", "number of order parameters to create");
  params.addRequiredParam<std::string>("var_name_base","specifies the base name of the variables");

  return params;
}

Tricrystal2CircleGrainsICAction::Tricrystal2CircleGrainsICAction(const std::string & name, InputParameters params)
  :Action(name, params),
   _var_name_base(getParam<std::string>("var_name_base")),
   _crys_num(getParam<unsigned int>("crys_num"))
{}

void
Tricrystal2CircleGrainsICAction::act()
{
#ifdef DEBUG
  Moose::err << "Inside the Tricrystal2CircleGrainsICAction Object\n";
#endif

// Loop through the number of order parameters


  for (unsigned int crys = 0; crys<_crys_num; crys++)
  {
    //Create variable names
    std::string var_name = _var_name_base;
    std::stringstream out;
    out << crys;
    var_name.append(out.str());

    //Set parameters for BoundingBoxIC
    InputParameters poly_params = _factory.getValidParams("Tricrystal2CircleGrainsIC");
    poly_params.set<VariableName>("variable") = var_name;
    poly_params.set<unsigned int>("crys_num") = _crys_num;
    poly_params.set<unsigned int>("crys_index") = crys;


    //Add initial condition
    _problem->addInitialCondition("Tricrystal2CircleGrainsIC", "InitialCondition", poly_params);
  }

}
