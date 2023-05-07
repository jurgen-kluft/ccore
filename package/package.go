package ccore

import (
	cbase "github.com/jurgen-kluft/cbase/package"
	"github.com/jurgen-kluft/ccode/denv"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

// GetPackage returns the package object of 'ccore'
func GetPackage() *denv.Package {
	// Dependencies
	unittestpkg := cunittest.GetPackage()
	cbasepkg := cbase.GetPackage()

	// The main (ccore) package
	mainpkg := denv.NewPackage("ccore")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(cbasepkg)

	// 'ccore' library
	mainlib := denv.SetupDefaultCppLibProject("ccore", "github.com\\jurgen-kluft\\ccore")
	mainlib.Dependencies = append(mainlib.Dependencies, cbasepkg.GetMainLib())

	// 'ccore' unittest project
	maintest := denv.SetupDefaultCppTestProject("ccore_test", "github.com\\jurgen-kluft\\ccore")
	maintest.Dependencies = append(maintest.Dependencies, unittestpkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, cbasepkg.GetMainLib())
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)
	return mainpkg
}
