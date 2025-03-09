#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// Definições dos pinos (PD3, PD6, PD7 como saídas; PD2 como entrada)
#define rele 3
#define sensor 2
#define ledVermelho 6
#define ledVerde 7

volatile int counter = 0;

void setup_timer2() {
    cli();
    TCCR2B = (1 << CS22);    // Prescaler 64
    TIMSK2 = (1 << TOIE2);    // Habilita interrupção por overflow
    TCNT2 = 5;                // Inicializa Timer2 para contar 5 ciclos
    sei();
}

void setup() {
    DDRD |= (1 << rele) | (1 << ledVermelho) | (1 << ledVerde); // PD3, PD6, PD7 como saídas
    DDRD &= ~(1 << sensor);   // PD2 como entrada

    // Estado inicial: rele ligado, ledVermelho aceso, ledVerde apagado
    PORTD = 0b01001000;       // PD3 (bit3) e PD6 (bit6) em HIGH; PD7 (bit7) em LOW

    setup_timer2();
}

int main(void) {
    setup();
    while (1) {}
}

ISR(TIMER2_OVF_vect) {
    TCNT2 = 5; // Reinicia o Timer2 para 5 (overflow após 251 ciclos)
    counter++;

    if (counter >= 500) { // 500ms (500 * ~1ms)
        counter = 0;

        bool solo_umido = PIND & (1 << sensor); // Lê PD2

        if (solo_umido) {
            // Solo úmido: rele desligado, ledVerde aceso, ledVermelho apagado
            PORTD = 0b10000000; // PD7 (bit7) em HIGH; PD3 e PD6 em LOW
        } else {
            // Solo seco: rele ligado, ledVermelho aceso, ledVerde apagado
            PORTD = 0b01001000; // PD3 (bit3) e PD6 (bit6) em HIGH; PD7 em LOW
        }
    }
}
