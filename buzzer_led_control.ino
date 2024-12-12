// Buzzer and LED control
#define PORTD_REG (*(volatile uint8_t *)0x2B)  // PORTD
#define DDRD_REG (*(volatile uint8_t *)0x2A)   // DDRD
#define PIND_REG (*(volatile uint8_t *)0x29)   // PIND

// SPI registers for slave mode
#define SPCR_REG (*(volatile uint8_t *)0x2C)   // SPI Control Register
#define SPSR_REG (*(volatile uint8_t *)0x2E)   // SPI Status Register
#define SPDR_REG (*(volatile uint8_t *)0x2F)   // SPI Data Register

// Define the control pins
#define CONTROL_PIN 7          // Buzzer control
#define LED_NORMAL_PIN 6       // Normal light
#define LED_EMERGENCY_PIN 5    // Emergency blinking light

bool emergencyMode = false;  // Flag for emergency mode

void setup() {
    // Initialize SPI in slave mode
    SPCR_REG = (1 << SPE) | (1 << SPIE); // Enable SPI and SPI interrupt (SPE = 1, SPIE = 1)

    // Configure control pins as output (buzzer and LEDs)
    DDRD_REG |= (1 << CONTROL_PIN) | (1 << LED_NORMAL_PIN) | (1 << LED_EMERGENCY_PIN);

    // Default state: Normal light ON, others OFF
    PORTD_REG |= (1 << LED_NORMAL_PIN);
    PORTD_REG &= ~((1 << CONTROL_PIN) | (1 << LED_EMERGENCY_PIN));
}

void loop() {
    // Check if data is received over SPI (SPI interrupt flag)
    if (SPSR_REG & (1 << SPIF)) {
        // Read the received byte from the SPI data register
        char command = SPDR_REG;

        // Process the command
        switch (command) {
            case '1':  // Emergency mode
                emergencyMode = true;
                break;

            case '0':  // Normal mode
                emergencyMode = false;
                PORTD_REG &= ~((1 << CONTROL_PIN) | (1 << LED_EMERGENCY_PIN));  // Turn off buzzer and emergency light
                PORTD_REG |= (1 << LED_NORMAL_PIN);  // Turn on normal light
                break;

            default:  // Handle invalid input
                break;
        }
    }

    // Emergency mode behavior
    if (emergencyMode) {
        PORTD_REG |= (1 << LED_EMERGENCY_PIN) | (1 << CONTROL_PIN);  // Turn on emergency light and buzzer
        delay(200);  // Blink/beep interval
        PORTD_REG &= ~((1 << LED_EMERGENCY_PIN) | (1 << CONTROL_PIN));  // Turn off emergency light and buzzer
        delay(200);  // Blink/beep interval
    }

    // Small delay for stability
    delay(100);
}