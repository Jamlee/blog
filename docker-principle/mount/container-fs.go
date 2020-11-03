package main

import (
	"fmt"
	"os"
	"path/filepath"
	"syscall"
)

func checkRootfs(rootfsPath string) {
	if _, err := os.Stat(rootfsPath); os.IsNotExist(err) {
		fmt.Printf("rootfsPath %s is not found you may need to download it",rootfsPath)
		os.Exit(1)
	}
}

//implement pivot_root by syscall
func pivotRoot(newroot string) error {

	preRoot := "/.pivot_root"
	putold := filepath.Join(newroot,preRoot) //putold:/tmp/ns-proc/rootfs/.pivot_root


	// pivot_root requirement that newroot and putold must not be on the same filesystem as the current root
	//current root is / and new root is /tmp/ns-proc/rootfs and putold is /tmp/ns-proc/rootfs/.pivot_root
	//thus we bind mount newroot to itself to make it different
	//try to comment here you can see the error
	if err := syscall.Mount(newroot, newroot, "", syscall.MS_BIND|syscall.MS_REC, ""); err != nil {
		fmt.Printf("mount newroot:%s to itself error \n",newroot)
		return err
	}

	// create putold directory, equal to mkdir -p xxx
	if err := os.MkdirAll(putold, 0700); err != nil {
		fmt.Printf("create putold directory %s erro \n",putold)
		return err
	}

	// call pivot_root
	if err := syscall.PivotRoot(newroot, putold); err != nil {
		fmt.Printf("call PivotRoot error, newroot:%s,putold:%s \n",newroot,putold)
		return err
	}

	// ensure current working directory is set to new root
	if err := os.Chdir("/"); err != nil {
		return err
	}

	// umount putold, which now lives at /.pivot_root
	putold = preRoot
	if err := syscall.Unmount(putold, syscall.MNT_DETACH); err != nil {
		fmt.Printf("umount putold:%s error \n",putold)
		return err
	}

	// remove putold
	if err := os.RemoveAll(putold); err != nil {
		fmt.Printf("remove putold:%s error \n",putold)
		return err
	}

	return nil
}


func mountProc(newroot string) error {
	source := "proc"
	target := filepath.Join(newroot, "/proc")
	fstype := "proc"
	flags := 0
	data := ""

	os.MkdirAll(target, 0755)
	if err := syscall.Mount(
		source,
		target,
		fstype,
		uintptr(flags),
		data,
	); err != nil {
		return err
	}

	return nil
}
