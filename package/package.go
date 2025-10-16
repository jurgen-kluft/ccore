package ccore

import (
	denv "github.com/jurgen-kluft/ccode/denv"
	csdk "github.com/jurgen-kluft/csdk/package"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

const (
	repo_path = "github.com\\jurgen-kluft\\"
	repo_name = "ccore"
)

// GetPackage returns the package object of 'ccore'
func GetPackage() *denv.Package {
	// Dependencies
	sdk_pkg := csdk.GetPackage()
	unittest_pkg := cunittest.GetPackage()

	// The main (ccore) package
	main_pkg := denv.NewPackage(repo_path, repo_name)
	main_pkg.AddPackage(sdk_pkg)
	main_pkg.AddPackage(unittest_pkg)

	// 'ccore' library
	mainlib := denv.SetupCppLibProject(main_pkg, repo_name)
	mainlib.AddDependencies(sdk_pkg.GetMainLib())

	// 'ccore' library for unittest
	testlib := denv.SetupCppTestLibProject(main_pkg, repo_name)

	// 'ccore' unittest project
	maintest := denv.SetupCppTestProject(main_pkg, repo_name)
	maintest.AddDependencies(unittest_pkg.GetMainLib())
	maintest.AddDependency(testlib)

	main_pkg.AddMainLib(mainlib)
	main_pkg.AddTestLib(testlib)
	main_pkg.AddUnittest(maintest)
	return main_pkg
}
