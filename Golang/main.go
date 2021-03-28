package main


import (
    "encoding/json"
	"fmt"
    "log"
	"io/ioutil"
	"net/http"
)

type JSONObj struct {
	Text string `json:"text"`
}


func main() {
    http.HandleFunc("/", index)
    http.HandleFunc("/error", error422)
    http.HandleFunc("/json", jsonFunc)

    fmt.Printf("Server is started at http://localhost:8080 \n")
    if err := http.ListenAndServe(":8080", nil); err != nil {
        log.Fatal(err)
    }
}

// Testing writing to screen
func index(w http.ResponseWriter, r *http.Request) {
	// This endpoint writes HELLO WORLD! to the screen
    w.Write([]byte(`HELLO WORLD! Please visit:
	http://localhost:8080/error
	http://localhost:8080/json`))
}

// Testing 422 error
func error422(w http.ResponseWriter, r *http.Request) {
	// This endpoint sends back a 422 error. 
    w.WriteHeader(http.StatusUnprocessableEntity)
	w.Write([]byte("Check in web console for status code. Should be 422"))
}

// Testing POSTjson 
func jsonFunc(w http.ResponseWriter, r *http.Request) {
    // Initialize empty toWatch
	var myJsonObj JSONObj

	// Parse request for the json which contains the file
	body, _ := ioutil.ReadAll(r.Body)
	errJSON := json.Unmarshal([]byte(body), &myJsonObj)
	if errJSON != nil {
		w.WriteHeader(http.StatusUnprocessableEntity)
		w.Write([]byte(`Couldn't understand the JSON body. If cURL is available, use command:
		curl -X POST -H "Content-Type: application/json" -d '{"text":"json body"}' http://localhost:8080/json`))
		return
	}
	defer r.Body.Close()

	// This endpoint sends back the json sent. 
    toDisplay := fmt.Sprintf("The json that was recieved:\n %+v", myJsonObj)
	w.Write([]byte(toDisplay))
}