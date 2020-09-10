package main

import (
	"log"
	"net/http"
)

func HelloServer(w http.ResponseWriter, req *http.Request) {
	w.Header().Set("Content-Type", "text/plain")
	w.Write([]byte("This is an example server.\n"))
}

func main() {
	http.HandleFunc("/hello", HelloServer)
	err := http.ListenAndServeTLS(":443", "_wildcard.zvos.zoomlion.com.pem",
		"_wildcard.zvos.zoomlion.com-key.pem", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}