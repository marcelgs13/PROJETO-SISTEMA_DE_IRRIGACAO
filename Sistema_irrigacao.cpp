#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

// Definições dos pinos
#define rele 3
#define sensor 2
#define ledVermelho 6
#define ledVerde 7

// Variáveis globais para controle de tempo
volatile int counter = 0; // Contador para o intervalo de leitura do sensor

void setup_timer2()
{
    cli();

    // Configura o Timer2 com prescaler de 64
    TCCR2B = (1 << CS22);
    TIMSK2 = (1 << TOIE2);
    TCNT2 = 5;

    sei();
}

void setup()
{
    // Configura os pinos como saída (1) ou entrada (0)
    DDRD |= (1 << rele) | (1 << ledVermelho) | (1 << ledVerde);
    DDRD &= ~(1 << sensor);

    // Estado inicial: relé ligado (irrigação ativada), LED vermelho aceso (solo seco), LED verde apagado
    PORTD |= (1 << rele);
    PORTD |= (1 << ledVermelho);
    PORTD &= ~(1 << ledVerde);

    // Configura o Timer2
    setup_timer2();
}

int main(void)
{
    setup(); // Configura os pinos e o Timer2

    while (1)
    {
        // O loop principal não faz nada, pois tudo é controlado pela interrupção
    }
}

// Rotina de interrupção do Timer2
ISR(TIMER2_OVF_vect)
{
    TCNT2 = 5;
    counter++;

    // Verifica se passaram 500ms (500 incrementos de ~1ms)
    if (counter >= 500)
    {
        counter = 0;

        // Lê o sensor de umidade
        bool solo_umido = PIND & (1 << sensor);

        if (solo_umido)
        { // Se estiver úmido
            PORTD &= ~(1 << rele);
            PORTD &= ~(1 << ledVermelho);
            PORTD |= (1 << ledVerde);
        }
        else
        { // Se estiver seco
            PORTD |= (1 << rele);
            PORTD |= (1 << ledVermelho);
            PORTD &= ~(1 << ledVerde);
        }
    }
