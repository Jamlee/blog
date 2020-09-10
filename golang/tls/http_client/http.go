package main
import (
	"net/http"
	"log"
)

func main() {
	_, err := http.Get("https://localhost:443/hello")
	if err != nil {
		log.Print(err)
	}
}