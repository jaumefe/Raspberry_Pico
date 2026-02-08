package main

import (
	"fmt"
	"time"

	"github.com/fxamacker/cbor/v2"
	"go.bug.st/serial"
)

func main() {
	mode := serial.Mode{
		BaudRate: 9600,
		DataBits: 8,
		Parity:   serial.NoParity,
		StopBits: serial.OneStopBit,
	}

	port, err := serial.Open("/dev/ttyACM0", &mode)
	if err != nil {
		panic(err)
	}
	port.SetReadTimeout(time.Second * 10)

	_, err = port.Write([]byte("LED_ON\n"))
	if err != nil {
		panic(err)
	}

	buf := make([]byte, 100)
	n, err := port.Read(buf)
	if err != nil {
		panic(err)
	}
	buf = buf[:n-1]

	var msi map[string]interface{}
	err = cbor.Unmarshal(buf, &msi)
	if err != nil {
		panic(err)
	}
	fmt.Println(msi)
}
