package main

import (
	"fmt"
	"time"

	"github.com/fxamacker/cbor/v2"
	"go.bug.st/serial"
)

type fridayMsg struct {
	Module string `cbor:"module"`
	Name   string `cbor:"name"`
	Type   string `cbor:"type"`
	Value  int    `cbor:"value"`
}

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

	msg := fridayMsg{
		Module: "gpio",
		Name:   "led",
		Type:   "cmd",
		Value:  1,
	}

	cborData, err := cbor.Marshal(msg)
	if err != nil {
		panic(err)
	}

	length := len(cborData)
	fmt.Printf("CBOR data length: %d bytes\n", length)
	l1 := (length >> 8) & 0xFF
	l2 := length & 0xFF
	fmt.Printf("Length bytes: %02X %02X\n", l1, l2)
	fridayMsg := []byte{0x46, 0x01, byte(l1), byte(l2)}
	fridayMsg = append(fridayMsg, cborData...)

	_, err = port.Write(fridayMsg)
	if err != nil {
		panic(err)
	}

	buf := make([]byte, 512)
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
