// Uno: A0 potentiometer -> 0..100 Hz square wave on D3, shown on LCD1602 (I2C)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const uint8_t OUT_PIN = 3;
const uint8_t LCD_ADDR = 0x27;  // try 0x3F if the display stays blank
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

int currentHz = -1;

// Timer1 CTC, prescaler 256 -> 62500 ticks/s; ISR toggles D3 (hardware-timed, no LCD jitter)
ISR(TIMER1_COMPA_vect) {
  PIND = _BV(PD3);  // writing 1 to PIN toggles the output
}

void setFrequency(int hz) {
  if (hz <= 0) {
    TIMSK1 &= ~_BV(OCIE1A);
    digitalWrite(OUT_PIN, LOW);
    return;
  }
  uint16_t top = (uint16_t)(62500UL / (2UL * hz)) - 1;  // two toggles per period
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = _BV(WGM12) | _BV(CS12);
  OCR1A = top;
  if (TCNT1 > top) TCNT1 = 0;
  TIMSK1 |= _BV(OCIE1A);
  interrupts();
}

void setup() {
  pinMode(OUT_PIN, OUTPUT);
  digitalWrite(OUT_PIN, LOW);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Frequency:");
}

void loop() {
  // average a few samples to reduce jitter
  uint32_t sum = 0;
  for (uint8_t i = 0; i < 8; i++) sum += analogRead(A0);
  int hz = (int)((sum / 8UL * 100UL + 511UL) / 1023UL);  // rounded 0..100

  if (hz != currentHz) {
    currentHz = hz;
    setFrequency(hz);
    lcd.setCursor(0, 1);
    lcd.print(hz);
    lcd.print(" Hz   ");
  }
  delay(20);
}
