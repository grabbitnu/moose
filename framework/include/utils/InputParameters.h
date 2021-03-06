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

#ifndef INPUTPARAMETERS_H
#define INPUTPARAMETERS_H

#include <vector>
#include <set>
#include <map>

// libMesh
#include "libmesh/parameters.h"

#include "MooseError.h"
#include "MooseTypes.h"
#include "MooseEnum.h"
#include "MooseUtils.h"

class MooseObject;
class GlobalParamsAction;
class Action;
class Parser;
class Problem;
class MooseApp;

class InputParameters;

template<class T>
InputParameters validParams();

/**
 *
 */
class InputParameters : public Parameters
{
public:
  InputParameters(const InputParameters &rhs);
  InputParameters(const Parameters &rhs);

  virtual ~InputParameters(){}

  virtual void clear();

  /**
   * This method adds a description of the class that will be displayed
   * in the input file syntax dump
   */
  void addClassDescription(const std::string &doc_string);

  /**
   * Returns the class description
   */
  std::string getClassDescription() const;

  /**
   * Override from libMesh to set user-defined attributes on our parameter
   */
  virtual void set_attributes(const std::string &name, bool inserted_only);

  /**
   * Returns a writable reference to the named parameters.  Note: This is not a virtual
   * function! Use caution when comparing to the parent class implementation
   */
  template <typename T> T & set (const std::string&);

  /**
   * Verifies that the requested parameter exists and is not NULL and returns it to the caller.
   * The template parameter must be a pointer or an error will be thrown.
   */
  template <typename T> T getCheckedPointerParam(const std::string &name, const std::string &error_string="");

  /**
   * This method adds a parameter and documentation string to the InputParameters
   * object that will be extracted from the input file.  If the parameter is
   * missing in the input file, and error will be thrown
   */
  template <typename T>
  void addRequiredParam(const std::string &name, const std::string &doc_string);

  /**
   * This version of addRequiredParam is here for a consistent use with MooseEnum.  Use of
   * this function for any other type will throw an error.
   */
  template <typename T>
  void addRequiredParam(const std::string &name, const T &moose_enum, const std::string &doc_string);

  /**
   * These methods add an option parameter and a documentation string to the InputParameters object.
   * The first version of this function takes a default value which is used if the parameter
   * is not found in the input file.  The second method will leave the parameter uninitialized
   * but can be checked with "isValid" before use
   */
  template <typename T>
  void addParam(const std::string &name, const T & value, const std::string &doc_string);
  template <typename T>
  void addParam(const std::string &name, const std::string &doc_string);

  /**
   * Method for adding a input parameter that expects a postprocessor name, but provides a default value
   * in the case where the postprocessor name provided is not valid or not provided.
   * @param name The name of the input file parameter that is expecting a PostprocessorName
   * @param default_value The default value to use in the case that the postprocessor does not exist
   * @param doc_string Documentation string for this parameter
   *
   * This parameter works with the PostprocessorInterface to return the default value when getPostprocessorValue is called
   */
  void addPostprocessor(const std::string & name, Real default_value, const std::string & doc_string);

  /**
   * These methods add an option parameter and with a customer type to the InputParameters object.  The custom
   * type will be output in YAML dumps and can be used within the GUI application.
   */
  template <typename T>
  void addRequiredCustomTypeParam(const std::string &name, const std::string &custom_type, const std::string &doc_string);
  template <typename T>
  void addCustomTypeParam(const std::string &name, const T &value, const std::string &custom_type, const std::string &doc_string);
  template <typename T>
  void addCustomTypeParam(const std::string &name, const std::string &custom_type, const std::string &doc_string);

  /**
   * These method adds a parameter to the InputParameters object which can be retrieved
   * like any other parameter.  This parameter however is not printed in the Input file syntax
   * dump or web page dump so does not take a documentation string.  The first version
   * of this function takes an optional default value.
   */
  template <typename T>
  void addPrivateParam(const std::string &name, const T &value);
  template <typename T>
  void addPrivateParam(const std::string &name);

  /**
   * Add parameters for retrieval from the command line.
   *
   * NOTE: This ONLY works for App objects!  This is not valid for normal MOOSE objects!
   *
   * @param name The name of the parameter
   * @param syntax Space separated list of command-line switch syntax that can set this option
   * @param doc_string Documentation.  This will be shown for --help
   */
  template <typename T>
  void addRequiredCommandLineParam(const std::string &name, const std::string &syntax, const std::string &doc_string);
  template <typename T>
  void addCommandLineParam(const std::string &name, const std::string &syntax, const std::string &doc_string);
  template <typename T>
  void addCommandLineParam(const std::string &name, const std::string &syntax, const T &value, const std::string &doc_string);

  /**
   * Add and mark a parameter for deprecation.  This will allow us to assist users as API changes are made.  If the user
   * supplies a value or even uses the default, a warning will be printed.
   *
   * @param name The name of the parameter
   * @param doc_string Documentation.  This will be shown for --help
   * @param deprecation_message The message that will will print about why this param was deprecated.  It might mention the "new way".
   */
  template <typename T>
  void addRequiredDeprecatedParam(const std::string &name, const std::string &doc_string, const std::string &deprecation_message);

  /**
   * @param name The name of the parameter
   * @param value The default value of this parameter if it requires one
   * @param doc_string Documentation.  This will be shown for --help
   * @param deprecation_message The message that will will print about why this param was deprecated.  It might mention the "new way".
   */
  template <typename T>
  void addDeprecatedParam(const std::string &name, const T &value, const std::string &doc_string, const std::string &deprecation_message);

  template <typename T>
  void addDeprecatedParam(const std::string &name, const std::string &doc_string, const std::string &deprecation_message);

  /**
   * Add and mark a parameter as removed.  If the user supplies this parameter then an error will be thrown.
   *
   * @param name The name of the parameter
   * @param removed_message The message that will will print about why this param was removed.
   */
  template <typename T>
  void addRemovedParam(const std::string &name, const std::string &removed_message);

  /**
   * This method checks to make sure that we aren't adding a parameter with the same name but a different type.  It
   * throws a MooseError if an inconsistent type is detected. While this state is supported by libMesh it brings
   * nothing but blood and tears for those who try ;)
   *
   * @param name the name of the parameter
   */
  template <typename T>
  void checkConsistentType(const std::string &name) const;

  /**
   * Get the syntax for a command-line parameter
   */
  std::vector<std::string> getSyntax(const std::string &name);

  /**
   * This method takes a space delimited list of parameter names and adds them to the specified group name.
   * This information is used in the GUI to group parameters into logical sections.
   */
  void addParamNamesToGroup(const std::string &space_delim_names, const std::string group_name);


  /**
   * This method retrieves the group name for the passed parameter name if one exists.  Otherwise an
   * empty string is returned.
   */
  std::string getGroupName(const std::string &param_name) const;


  /**
   * This method suppresses an inherited parameter so that it isn't required or valid
   * in the derived class. The parameter is added to the private parameter list.
   * Suppressing a parameter can have dire consequences.
   * Use at your own risk!
   */
  template <typename T>
  void suppressParameter(const std::string &name);

  /**
   * Changes the parameter to be required.
   * @param name The parameter name
   */
  template <typename T>
  void makeParamRequired(const std::string &name);

  /**
   * This method adds a coupled variable name pair.  The parser will look for variable
   * name pair in the input file and can return a reference to the storage location
   * for the coupled variable if found
   */
  void addCoupledVar(const std::string &name, const std::string &doc_string);

  /**
   * This method adds a coupled variable name pair.  The parser will look for variable
   * name pair in the input file and can return a reference to the storage location
   * for the coupled variable if found
   *
   * Also - you can provide a default value for this variable in the case that an actual variable is not provided.
   */
  void addCoupledVar(const std::string &name, const Real value, const std::string &doc_string);

  /**
   * Utility functions for retrieving one of the MooseTypes variables into the common "string" base class.
   * Scalar and Vector versions are supplied
   */
  std::string getMooseType(const std::string &name) const;
  std::vector<std::string> getVecMooseType(const std::string &name) const;

  /**
   * This method adds a coupled variable name pair.  The parser will look for variable
   * name pair in the input file and can return a reference to the storage location
   * for the coupled variable.  If the coupled variable is not supplied in the input
   * file, and error is thrown
   */
  void addRequiredCoupledVar(const std::string &name, const std::string &doc_string);

  /**
   * Returns the documentation string for the specified parameter name
   */
  std::string getDocString(const std::string &name) const;

  /**
   * Returns a boolean indicating whether the specified parameter is required or not
   */
  bool isParamRequired(const std::string &name) const;

  /**
   * This method returns parameters that have been initialized in one fashion or another,
   * i.e. The value was supplied as a default argument or read and properly converted from
   * the input file
   */
  bool isParamValid(const std::string &name) const;

  /**
   * This method returns true if all of the parameters in this object are valid
   * (i.e. isParamValid(name) == true - for all parameters)
   */
  bool areAllRequiredParamsValid() const;

  /**
   * Prints the type of the requested parameter by name
   */
  std::string type(const std::string &name);

  /**
   * Returns a boolean indicating whether the specified parameter is private or not
   */
  bool isPrivate(const std::string &name) const;

  /**
   * This method must be called from every base "Moose System" to create linkage with the Action System.
   * See "Moose.C" for the registerMooseObjectTask() calls.
   */
  void registerBase(const std::string &value);

  /**
   * This method is here to indicate which Moose types a particular Action may build. It takes a space
   * delimited list of registered MooseObjects.  TODO: For now we aren't actually checking this list
   * when we build objects. Since individual actions can do whatever they want it's not exactly trivial
   * to check this without changing the user API.  This function properly restricts the syntax and yaml dumps.
   */
  void registerBuildableTypes(const std::string &names);

  /**
   * Returns the list of buildable types as a std::vector<std::string>
   */
  const std::vector<std::string> & getBuildableTypes() const;

  /**
   * Mutators for controlling whether or not the outermost level of syntax will be collapsed when printed.
   */
  void collapseSyntaxNesting(bool collapse);
  bool collapseSyntaxNesting() const;

  /**
   * Mutators for controlling whether or not the outermost level of syntax will be collapsed when printed.
   */
  void mooseObjectSyntaxVisibility(bool visibility);
  bool mooseObjectSyntaxVisibility() const;

  /**
   * Copy and Copy/Add operators for the InputParameters object
   */
  using Parameters::operator=;
  using Parameters::operator+=;
  InputParameters & operator=(const InputParameters &rhs);
  InputParameters & operator+=(const InputParameters &rhs);

  /**
   * This function checks parameters stored in the object to make sure they are in the correct
   * state as the user expects:
   *   Required parameters are verified as valid meaning that they were either initialized when
   *   they were created, or were read from an input file or some other valid source
   */
  void checkParams(const std::string & prefix) const;

  /**
   * Methods returning iterators to the coupled variables names stored in this
   * InputParameters object
   */
  inline std::set<std::string>::const_iterator coupledVarsBegin()
  {
    return _coupled_vars.begin();
  }
  inline std::set<std::string>::const_iterator coupledVarsEnd()
  {
    return _coupled_vars.end();
  }

  /**
   * Get the default value for an optionally coupled variable.
   *
   * @param coupling_name The name of the coupling parameter to get the default value for.
   */
  Real defaultCoupledValue(std::string coupling_name);

  /**
   * Get the default value for a postprocessor added with addPostprocessor
   * @param name The name of the postprocessor
   * @return The default value for the postprocessor
   */
  PostprocessorValue & defaultPostprocessorValue(const std::string & name);

  /**
   * Returns true if a default PostprocessorValue is defined
   * @param name The name of the postprocessor
   * @return True if a default value exists
   */
  bool hasDefaultPostprocessorValue(const std::string & name) const;

  /*
   * Method for applying common parameters
   * @param common The set of parameters to apply to the parameters store in this object
   *
   * The parameters supplied as an argument are set if:
   *  (1) The common parameter exists as a local parameter
   *  (2) The local parameter is not valid
   *  (3) The local parameter is not private
   *  (4) The common parameter is valid
   *  (5) The common parameter is not private
   *
   * Output object have a set of common parameters that are passed
   * down to each of the output object created. This method is used for
   * apply those common parameters, but only if the parameter exists on the object
   * and it has yet to be set to a valid value.
   *
   * @see CommonOutputAction AddOutputAction
   */
  void applyParameters(const InputParameters & common);

  // These are the only objects allowed to _create_ InputParameters
  friend InputParameters validParams<MooseObject>();
  friend InputParameters validParams<Action>();
  friend InputParameters validParams<Problem>();
  friend InputParameters emptyInputParameters();
  friend InputParameters validParams<MooseApp>();

private:
  // Private constructor so that InputParameters can only be created in certain places.
  InputParameters();

  /// The documentation strings for each parameter
  std::map<std::string, std::string> _doc_string;

  /// The custom type that will be printed in the YAML dump for a parameter if supplied
  std::map<std::string, std::string> _custom_type;

  /// Syntax for command-line parameters
  std::map<std::string, std::vector<std::string> > _syntax;

  /// The names of the parameters organized into groups
  std::map<std::string, std::string> _group;

  /// The parameter is used to restrict types that can be built.  Typically this
  /// is used for MooseObjectAction derived Actions.
  std::vector<std::string> _buildable_types;

  /// This parameter collapses one level of nesting in the syntax blocks.  It is used
  /// in conjuction with MooseObjectAction derived Actions.
  bool _collapse_nesting;

  /// This parameter hides derived MOOSE object types from appearing in syntax dumps
  bool _moose_object_syntax_visibility;

  /// The set of parameters that are required (i.e. will cause an abort if not supplied)
  std::set<std::string> _required_params;

  /**
   * The set of parameters either explicitly set or provided a default value when added
   * Note: We do not store MooseEnum names in valid params, instead we asked MooseEnums whether
   *       they are valid or not.
   */
  std::set<std::string> _valid_params;

  /// The set of parameters that will NOT appear in the the dump of the parser tree
  std::set<std::string> _private_params;

  /// The coupled variables set
  std::set<std::string> _coupled_vars;

  /// The default value for optionally coupled variables
  std::map<std::string, Real> _default_coupled_value;

  /// The default value for postprocessors
  std::map<std::string, PostprocessorValue> _default_postprocessor_value;

};

// Template and inline function implementations
template <typename T>
T &
InputParameters::set (const std::string& name)
{
  checkConsistentType<T>(name);

  if (!this->have_parameter<T>(name))
    _values[name] = new Parameter<T>;

  set_attributes(name, false);

  return libmesh_cast_ptr<Parameter<T>*>(_values[name])->set();
}

template <typename T>
T
InputParameters::getCheckedPointerParam(const std::string &name, const std::string &error_string)
{
  T param = this->get<T>(name);

  // Note: You will receive a compile error on this line if you attempt to pass a non-pointer template type to this method
  if (param == NULL)
    mooseError("Parameter " << name << " is NULL.\n" << error_string);
  return this->get<T>(name);
}

template <typename T>
void
InputParameters::addRequiredParam(const std::string &name, const std::string &doc_string)
{
  checkConsistentType<T>(name);

  InputParameters::insert<T>(name);
  _required_params.insert(name);
  _doc_string[name] = doc_string;
}

template <typename T>
void
InputParameters::addRequiredParam(const std::string & /*name*/, const T & /*value*/, const std::string & /*doc_string*/)
{
  mooseError("You cannot call addRequiredParam and supply a default value for this type, please use addParam instead");
}

template <typename T>
void
InputParameters::addParam(const std::string &name, const T &value, const std::string &doc_string)
{
  checkConsistentType<T>(name);

  InputParameters::set<T>(name) = value;                    // valid parameter is set by set_attributes
  _doc_string[name] = doc_string;
}

template <typename T>
void
InputParameters::addParam(const std::string &name, const std::string &doc_string)
{
  checkConsistentType<T>(name);

  InputParameters::insert<T>(name);
  _doc_string[name] = doc_string;
}

template <typename T>
void
InputParameters::addRequiredCustomTypeParam(const std::string &name, const std::string &custom_type, const std::string &doc_string)
{
  addRequiredParam<T>(name, doc_string);
  _custom_type[name] = custom_type;
}

template <typename T>
void
InputParameters::addCustomTypeParam(const std::string &name, const T &value, const std::string &custom_type, const std::string &doc_string)
{
  addParam<T>(name, value, doc_string);
  _custom_type[name] = custom_type;
}

template <typename T>
void
InputParameters::addCustomTypeParam(const std::string &name, const std::string &custom_type, const std::string &doc_string)
{
  addParam<T>(name, doc_string);
  _custom_type[name] = custom_type;
}

template <typename T>
void
InputParameters::addPrivateParam(const std::string &name)
{
  checkConsistentType<T>(name);

  InputParameters::insert<T>(name);
  _private_params.insert(name);
}

template <typename T>
void
InputParameters::addPrivateParam(const std::string &name, const T &value)
{
  checkConsistentType<T>(name);

  InputParameters::set<T>(name) = value;
  _private_params.insert(name);}

template <typename T>
void
InputParameters::addRequiredCommandLineParam(const std::string &name, const std::string &syntax, const std::string &doc_string)
{
  addRequiredParam<T>(name, doc_string);
  MooseUtils::tokenize(syntax, _syntax[name], 1, " \t\n\v\f\r");
}

template <typename T>
void
InputParameters::addCommandLineParam(const std::string &name, const std::string &syntax, const std::string &doc_string)
{
  addParam<T>(name, doc_string);
  MooseUtils::tokenize(syntax, _syntax[name], 1, " \t\n\v\f\r");
}

template <typename T>
void
InputParameters::addCommandLineParam(const std::string &name, const std::string &syntax, const T &value, const std::string &doc_string)
{
  addParam<T>(name, value, doc_string);
  MooseUtils::tokenize(syntax, _syntax[name], 1, " \t\n\v\f\r");
}


template <typename T>
void
InputParameters::checkConsistentType(const std::string &name) const
{
  // Do we have a paremeter with the same name but a different type?
  InputParameters::const_iterator it = _values.find(name);
  if (it != _values.end() && dynamic_cast<const Parameter<T>*>(it->second) == NULL)
    mooseError("Attempting to set parameter \"" << name << "\" with type (" << demangle(typeid(T).name()) << ")\nbut the parameter already exists as type (" << it->second->type() << ")");
}

template <typename T>
void
InputParameters::suppressParameter(const std::string &name)
{
  _required_params.erase(name);
  _valid_params.erase(name);
  _private_params.insert(name);
}

template <typename T>
void
InputParameters::addRequiredDeprecatedParam(const std::string &name, const std::string &doc_string, const std::string &deprecation_message)
{
  mooseWarning("The parameter " << name << " is deprecated.\n" << deprecation_message);

  addRequiredParam<T>(name, doc_string);
}

template <typename T>
void
InputParameters::addDeprecatedParam(const std::string &name, const T &value, const std::string &doc_string, const std::string &deprecation_message)
{
  mooseWarning("The parameter " << name << " is deprecated.\n" << deprecation_message);

  addParam<T>(name, value, doc_string);
}

template <typename T>
void
InputParameters::addDeprecatedParam(const std::string &name, const std::string &doc_string, const std::string &deprecation_message)
{
  mooseWarning("The parameter " << name << " is deprecated.\n" << deprecation_message);

  addParam<T>(name, doc_string);
}

template <typename T>
void
InputParameters::addRemovedParam(const std::string &name, const std::string &removed_message)
{
  mooseError("The parameter " << name << " has been removed.\n" << removed_message);
}

// Specializations for MooseEnum
template <>
inline
void
InputParameters::addRequiredParam<MooseEnum>(const std::string &name, const MooseEnum &moose_enum, const std::string &doc_string)
{
  InputParameters::set<MooseEnum>(name) = moose_enum;                    // valid parameter is set by set_attributes
  _required_params.insert(name);
  _doc_string[name] = doc_string;
}

template <>
inline
void
InputParameters::addRequiredParam<std::vector<MooseEnum> >(const std::string &name, const std::vector<MooseEnum> &moose_enums, const std::string &doc_string)
{
  InputParameters::set<std::vector<MooseEnum> >(name) = moose_enums;    // valid parameter is set by set_attributes
  _required_params.insert(name);
  _doc_string[name] = doc_string;
}

template <>
inline
void
InputParameters::addParam<MooseEnum>(const std::string & /*name*/, const std::string & /*doc_string*/)
{
  mooseError("You must supply a MooseEnum object when using addParam, even if the parameter is not required!");
}

template <>
inline
void
InputParameters::addParam<std::vector<MooseEnum> >(const std::string & /*name*/, const std::string & /*doc_string*/)
{
  mooseError("You must supply a vector of MooseEnum object(s) when using addParam, even if the parameter is not required!");
}

InputParameters emptyInputParameters();

#endif /* INPUTPARAMETERS_H */
