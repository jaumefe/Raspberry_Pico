package main

import (
	"fmt"
	"time"

	"github.com/fxamacker/cbor/v2"
	"go.bug.st/serial"
)

type fridayPayload struct {
	Module string `cbor:"module"`
	Name   string `cbor:"name"`
	Type   string `cbor:"type"`
	Value  int    `cbor:"value"`
}

type fridayMsg struct {
	Header  byte
	Version byte
	Length  uint16
	Payload []byte
}

var msgTest = []fridayPayload{
	// {
	// 	Module: "gpio",
	// 	Name:   "led",
	// 	Type:   "cmd",
	// 	Value:  1,
	// },
	// {
	// 	Module: "gpio",
	// 	Name:   "led",
	// 	Type:   "cmd",
	// 	Value:  0,
	// },
	// {
	// 	Module: "sensor",
	// 	Name:   "dps310",
	// 	Type:   "init",
	// 	Value:  1,
	// },
	// {
	// 	Module: "sensor",
	// 	Name:   "dps310",
	// 	Type:   "coeff",
	// 	Value:  1,
	// },
	// {
	// 	Module: "sensor",
	// 	Name:   "dps310",
	// 	Type:   "meas",
	// 	Value:  1,
	// },
	// {
	// 	Module: "sensor",
	// 	Name:   "sht4x",
	// 	Type:   "meas",
	// 	Value:  1,
	// },
	{
		Module: "sensor",
		Name:   "bme680",
		Type:   "init",
		Value:  1,
	},
	{
		Module: "sensor",
		Name:   "bme680",
		Type:   "calib",
		Value:  1,
	},
	{
		Module: "sensor",
		Name:   "bme680",
		Type:   "meas",
		Value:  1,
	},
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

	for i, msg := range msgTest {
		time.Sleep(time.Second * 5)

		cborData, err := cbor.Marshal(msg)
		if err != nil {
			panic(err)
		}

		length := len(cborData)
		l1 := (length >> 8) & 0xFF
		l2 := length & 0xFF
		fridayMsg := []byte{0x46, 0x01, byte(l1), byte(l2)}
		fridayMsg = append(fridayMsg, cborData...)
		// Print the message in hexadecimal format
		for _, b := range fridayMsg {
			fmt.Printf("%02X ", b)
		}
		fmt.Println()
		n, err := port.Write(fridayMsg)
		fmt.Println("Sent bytes:", n)
		if err != nil {
			panic(err)
		}

		if i == 0 {
			continue
		}

		buf := make([]byte, 512)
		n, err = port.Read(buf)
		fmt.Println("Received bytes:", n)
		if err != nil {
			panic(err)
		}

		// Parse Friday Message
		buf = buf[:n-1]
		if buf[0] != 0x46 {
			fmt.Println("Invalid header")
		}

		if buf[1] != 0x01 {
			fmt.Println("Invalid version")
		}

		length = int(buf[2])<<8 | int(buf[3])
		if length != n-4 {
			fmt.Println("Invalid length")
		}

		payload := buf[4 : 4+length]
		fmt.Printf("Payload: %X\n", payload)

		var msi map[string]interface{}
		err = cbor.Unmarshal(payload, &msi)
		if err != nil {
			panic(err)
		}
		fmt.Println(msi)
	}

}
