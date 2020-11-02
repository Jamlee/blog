// +build linux

package main

import (
	"fmt"
	"os"
	"os/exec"
	"syscall"

	"github.com/docker/docker/pkg/reexec"
)

func init() {

	fmt.Printf("arg0=%s,\n", os.Args[0])

	reexec.Register("initFuncName", func() {
		fmt.Printf("\n>> namespace setup code goes here <<\n\n")

		nsRun() //calling clone() to create new process goes here
	})

	if reexec.Init() {
		os.Exit(0)
	}
}

func nsRun() {
	cmd := exec.Command("sh")

	//set identify for this demo
	cmd.Env = []string{"PS1=-[namespace-process]-# "}
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	if err := cmd.Run(); err != nil {
		fmt.Printf("Error running the /bin/sh command - %s\n", err)
		os.Exit(1)
	}
}

func main() {

	// execve 调用 self 会使用之前的程序内存吗
	cmd := reexec.Command("initFuncName")

	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	cmd.SysProcAttr = &syscall.SysProcAttr{
		Cloneflags: syscall.CLONE_NEWNS |
			syscall.CLONE_NEWUTS |
			syscall.CLONE_NEWIPC |
			syscall.CLONE_NEWPID |
			syscall.CLONE_NEWNET |
			syscall.CLONE_NEWUSER,
		UidMappings: []syscall.SysProcIDMap{
			{
				ContainerID: 0,
				HostID:      os.Getuid(),
				Size:        1,
			},
		},
		GidMappings: []syscall.SysProcIDMap{
			{
				ContainerID: 0,
				HostID:      os.Getgid(),
				Size:        1,
			},
		},
	}

	// exec 底层使用的是 execve 系统调用
	if err := cmd.Run(); err != nil {
		fmt.Printf("Error running the reexec.Command - %s\n", err)
		os.Exit(1)
	}

}
