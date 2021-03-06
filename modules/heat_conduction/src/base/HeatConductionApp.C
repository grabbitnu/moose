#include "HeatConductionApp.h"
#include "Moose.h"
#include "AppFactory.h"

#include "AddSlaveFluxVectorAction.h"
#include "ConvectiveFluxFunction.h"
#include "GapConductance.h"
#include "GapConductanceConstraint.h"
#include "GapHeatPointSourceMaster.h"
#include "GapHeatTransfer.h"
#include "HeatConduction.h"
#include "HeatConductionTimeDerivative.h"
#include "HeatConductionMaterial.h"
#include "HeatConductionBC.h"
#include "BulkCoolantBC.h"
#include "ThermalContactAuxBCsAction.h"
#include "ThermalContactAuxKernelsAction.h"
#include "ThermalContactAuxVarsAction.h"
#include "ThermalContactBCsAction.h"
#include "ThermalContactDiracKernelsAction.h"
#include "ThermalContactMaterialsAction.h"
#include "HeatSource.h"
#include "ThermalCond.h"
#include "CoupledConvectiveFlux.h"

template<>
InputParameters validParams<HeatConductionApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

HeatConductionApp::HeatConductionApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(libMesh::processor_id());

  Moose::registerObjects(_factory);
  HeatConductionApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  HeatConductionApp::associateSyntax(_syntax, _action_factory);
}

HeatConductionApp::~HeatConductionApp()
{
}

void
HeatConductionApp::registerApps()
{
  registerApp(HeatConductionApp);
}

void
HeatConductionApp::registerObjects(Factory & factory)
{
  registerNamedKernel(HeatConductionKernel, "HeatConduction");
  registerKernel(HeatConductionTimeDerivative);
  registerKernel(HeatSource);
  registerBoundaryCondition(HeatConductionBC);
  registerBoundaryCondition(ConvectiveFluxFunction);
  registerBoundaryCondition(GapHeatTransfer);
  registerBoundaryCondition(BulkCoolantBC);
  registerBoundaryCondition(CoupledConvectiveFlux);
  registerMaterial(GapConductance);
  registerMaterial(HeatConductionMaterial);
  registerDiracKernel(GapHeatPointSourceMaster);
  registerPostprocessor(ThermalCond);
  registerConstraint(GapConductanceConstraint);
}

void
HeatConductionApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  // This registers an action to add the "slave_flux" vector to the system at the right time
  registerTask("add_slave_flux_vector", false);
  addTaskDependency("add_slave_flux_vector", "ready_to_init");
  addTaskDependency("init_problem", "add_slave_flux_vector");
  registerAction(AddSlaveFluxVectorAction, "add_slave_flux_vector");
  syntax.registerActionSyntax("AddSlaveFluxVectorAction", "ThermalContact/*");


  syntax.registerActionSyntax("ThermalContactAuxBCsAction",       "ThermalContact/*", "add_aux_kernel");
  syntax.registerActionSyntax("ThermalContactAuxKernelsAction",   "ThermalContact/*", "add_aux_kernel");
  syntax.registerActionSyntax("ThermalContactAuxVarsAction",      "ThermalContact/*", "add_aux_variable");
  syntax.registerActionSyntax("ThermalContactBCsAction",          "ThermalContact/*", "add_bc");
  syntax.registerActionSyntax("ThermalContactDiracKernelsAction", "ThermalContact/*", "add_dirac_kernel");
  syntax.registerActionSyntax("ThermalContactMaterialsAction",    "ThermalContact/*", "add_material");

  registerAction(ThermalContactAuxBCsAction,       "add_aux_kernel");
  registerAction(ThermalContactAuxKernelsAction,   "add_aux_kernel");
  registerAction(ThermalContactAuxVarsAction,      "add_aux_variablekernel");
  registerAction(ThermalContactBCsAction,          "add_bc");
  registerAction(ThermalContactDiracKernelsAction, "add_dirac_kernel");
  registerAction(ThermalContactMaterialsAction,    "add_material");
}
