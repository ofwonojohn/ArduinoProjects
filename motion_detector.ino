//motion detector// ATmega328P Register Definitions
#define DDRC    ((volatile uint8_t)(0x27))
#define PORTC   ((volatile uint8_t)(0x28))
#define PINC    ((volatile uint8_t)(0x26))

// Pin Definitions
#define PIR_PIN 0

void setup() {
    // Set PIR pin as input
    DDRC &= ~(1 << PIR_PIN);
    
    // Enable pull-up resistor
    PORTC |= (1 << PIR_PIN);
    
    Serial.begin(9600);
}

void loop() {
    // Read PIR sensor state directly from PIN register
    if (PINC & (1 << PIR_PIN)) {
        Serial.write('1');
    } else {
        Serial.write('0');
    }
    
    delay(1000);
}