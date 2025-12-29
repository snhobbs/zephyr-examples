# Console SCPI, Blinky, And ADC

- Uses the console interface to make an SCPI interface over UART
- Setup the ADC to do DMA based measurements optionally through an SPI interface and using the onboard ADC. Show that the switch can be made gracefully.


- General design
	+ Background sensors
		+ I2C, onboard ADC, etc 
	+ SPI ADC blasting data into a DMA buffer
	+ Use a struct to store sensor readings
	+ 14 bit at 3MSPS -> 48 MHz
		+ Use the scope style, keep the sample depth constant and change the sampling speed if required 
		+ 1ms is 3000 data points -> 6k RAM
		+ This can be processed directly if data is simple enough
		+ Buffer and stream out uart with a state machine
	+ Measurement work scheduled with workqueue
		+ Turn on LED, sleep, wakeup, digitize block A, turnoff, digitize block B. A short delay between the two is acceptable but should be minimized,
			+ Can be kept to only a couple clocks if done well.
			+ Could do 2x DMA tasks with one to the PIO for controlling the LED.

