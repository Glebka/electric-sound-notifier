#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>

#define SENSE_PIN (3)
#define BUZZER_PIN (4)

volatile bool main_power_active = false;
volatile bool previous_power_state = false;

void setup()
{
  // disable ADC
  ADCSRA = 0;

  // turn off various modules
  power_all_disable();

  // but enable the necessary ones
  power_timer0_enable();
  power_timer1_enable();

  GIMSK = 1 << PCIE;
  PCMSK = 1 << PCINT3;

  pinMode(SENSE_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void power_down()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_enable();
  sei();
  sleep_cpu();
}

void idle()
{
  set_sleep_mode(SLEEP_MODE_IDLE);
  cli();
  sleep_enable();
  sei();
  sleep_cpu();
}

bool is_bod()
{
  return MCUSR & 1 << BORF;
}

void reset_bod_flag()
{
  if (is_bod())
  {
    MCUSR ^= 1 << BORF;
  }
}

void play_main_power_active_sound()
{
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
}

void play_main_power_gone_sound()
{
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
  delay(100);
}

bool check_main_power()
{
  delay(2000);
  return digitalRead(SENSE_PIN);
}

void loop()
{
  if (is_bod())
  {
    reset_bod_flag();
    return;
  }
  main_power_active = check_main_power();
  if (previous_power_state != main_power_active)
  {
    previous_power_state = main_power_active;
    if (main_power_active)
    {
      play_main_power_active_sound();
    }
    else
    {
      play_main_power_gone_sound();
      power_down();
    }
  }
}

EMPTY_INTERRUPT(PCINT0_vect);