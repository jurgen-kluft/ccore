package ccore

import (
	denv "github.com/jurgen-kluft/ccode/denv"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

const (
	repo_path = "github.com\\jurgen-kluft\\"
	repo_name = "ccore"
)

// GetPackage returns the package object of 'ccore'
func GetPackage() *denv.Package {
	name := repo_name

	// Dependencies
	unittestpkg := cunittest.GetPackage()

	// The main (ccore) package
	mainpkg := denv.NewPackage(repo_path, repo_name)
	mainpkg.AddPackage(unittestpkg)

	// 'ccore' library
	mainlib := denv.SetupCppLibProject(mainpkg, name)

	// 'ccore' library for unittest
	testLib := denv.SetupCppTestLibProject(mainpkg, name)

	// 'ccore' unittest project
	maintest := denv.SetupCppTestProject(mainpkg, name)
	maintest.AddDependencies(unittestpkg.GetMainLib()...)
	maintest.AddDependency(testLib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddTestLib(testLib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
