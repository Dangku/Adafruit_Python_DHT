**Sunxi GPIO**

Gpio bank

	Hardware GPIO Bank, PA, PB, PC, ..., PL, PM
	
Bank index

	Index for each bank, 0, 1, 2, ..., 11, 12

	PA -> 0
	PB -> 1
	...
	PL -> 11
	PM -> 12
	
Gpio offset

	Gpio offset in each bank, for example

	PA4 -> offset is 4
	PH7 -> offset is 7

**GPIO Number**

gpio_number = Bank Index * 32 + Gpio offset, for example

	PH7 -> Bank Index is 7, Gpio offset is 7

	gpio_number = 7 * 32 + 7 = 231

	PL8 -> Bank Index is 11, Gpio offset is 8

	gpio_number = 11 * 32 + 8 = 360
