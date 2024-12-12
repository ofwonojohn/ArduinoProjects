//master code
#define PORTD_REG (*(volatile uint8_t *)0x2B)  // PORTD
#define DDRD_REG (*(volatile uint8_t *)0x2A)   // DDRD
#define PIND_REG (*(volatile uint8_t *)0x29)   // PIND

// SPI Register Definitions
#define SPCR_REG (*(volatile uint8_t *)0x2C)   // SPI Control Register
#define SPSR_REG (*(volatile uint8_t *)0x2D)   // SPI Status Register
#define SPDR_REG (*(volatile uint8_t *)0x2E)   // SPI Data Register
#define DDRB_REG (*(volatile uint8_t *)0x24)   // DDRB
#define PORTB_REG (*(volatile uint8_t *)0x25)  // PORTB

// SPI Pins
#define SS_LED 2      // PB2 - Slave Select for LED/Buzzer Arduino
#define SS_EEPROM 1   // PB1 - Slave Select for EEPROM Arduino
#define MOSI 3        // PB3 - MOSI
#define SCK 5         // PB5 - SCK

void setup() {
    // Initialize Serial communication
    Serial.begin(9600);

    // Initialize SPI as Master using registers
    DDRB_REG |= (1 << SS_LED) | (1 << SS_EEPROM) | (1 << MOSI) | (1 << SCK); // Set SS_LED, SS_EEPROM, MOSI, SCK as output
    PORTB_REG |= (1 << SS_LED) | (1 << SS_EEPROM); // Deselect both slaves (SS pins high)

    SPCR_REG = (1 << SPE) | (1 << MSTR) | (1 << SPR0);  // Enable SPI, Set as Master, Fosc/16

    // Notify initialization complete
    Serial.write("Master board initialized...\n");
}

void loop() {
    if (Serial.available() > 0) {
        char receivedCommand = Serial.read();  // Read command from the sensor Arduino via RX

        // Print received command to the serial monitor
        Serial.write("Received command from sensor: ");
        Serial.write(receivedCommand);
        Serial.write("\n");

        // Forward the command to the LED/Buzzer Arduino via SPI
        PORTB_REG &= ~(1 << SS_LED);         // Select LED/Buzzer Arduino (SS_LED low)
        SPDR_REG = receivedCommand;         // Send data
        while (!(SPSR_REG & (1 << SPIF)));  // Wait for transfer to complete
        PORTB_REG |= (1 << SS_LED);         // Deselect LED/Buzzer Arduino (SS_LED high)
        Serial.write("Forwarded to LED/Buzzer Arduino via SPI\n");

        // Forward the command to the EEPROM Arduino via SPI
        PORTB_REG &= ~(1 << SS_EEPROM);      // Select EEPROM Arduino (SS_EEPROM low)
        SPDR_REG = receivedCommand;         // Send data
        while (!(SPSR_REG & (1 << SPIF)));  // Wait for transfer to complete
        PORTB_REG |= (1 << SS_EEPROM);      // Deselect EEPROM Arduino (SS_EEPROM high)
        Serial.write("Forwarded to EEPROM Arduino via SPI\n");
    }

    // Small delay for stability
    delay(100);
}