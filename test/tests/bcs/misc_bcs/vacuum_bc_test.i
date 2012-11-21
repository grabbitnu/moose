[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  nz = 0
  zmin = 0
  zmax = 0
  elem_type = QUAD4
[]

[Variables]
  active = 'u'

  [./u]
    order = FIRST
    family = LAGRANGE    
  [../]
[]

[Kernels]
  active = 'diff'

  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[BCs]
  active = 'left right top'

  [./left]
    type = DirichletBC
    variable = u
    boundary = 3
    value = 0.0
  [../]

  [./right]
    type = NeumannBC
    variable = u
    boundary = 1
    value = 2.0
  [../]

  [./top]
    type = VacuumBC
    variable = u
    boundary = 2
    alpha = 5.0
  [../]
[]

[Executioner]
  type = Steady
  petsc_options = '-snes_mf_operator'
[]

[Output]
  output_initial = true
  interval = 1
  exodus = true
  print_linear_residuals = true
  perf_log = true
[]