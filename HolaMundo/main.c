//**************************************************************************************************************
/**
 * LABORATORIO 7 ELECTRÓNICA DIGITAL II
 * SECCION: 31
 * JUAN PABLO VALENZUELA, CARNET: 18057
 * DESCRIPCION:
 * USO Y CONFIGURACIÓN DE LAS INTERRUPCIONES Y EL MODULO UART
 */
//**************************************************************************************************************
// Librerías necesarias
//**************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#define XTAL 16000000

//**************************************************************************************************************
// Variables Globales
//**************************************************************************************************************
uint32_t i = 0;
uint32_t ui32Period;
uint32_t receive = 114;
unsigned int op = 0;

//**************************************************************************************************************
// Prototipos de Funciones
//**************************************************************************************************************
void uart_test(void);
void InitUART(void);
void Toggle(void);

//**************************************************************************************************************
// Función Principal
//**************************************************************************************************************
int main(void)
{
    // Se setea oscilador externo de 16MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);  //Reloj configurado a 40MHz

    // Se asigna reloj a puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


    // Se establecen como salidas los pines 1, 2 y 3 del puerto F
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    /*
     * Pines puerto F:
     * 1. Rojo
     * 2. Azul
     * 3. Verde
     */


    // Se inicializa la comunicación UART
    InitUART();
    //Se envía un mensaje de prueba
    uart_test();

    //Configuración del timer 0
    // Se habilita el reloj para el timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configuración del Timer 0 como temporizador períodico
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // Se calcula el período para el temporizador (1/2 seg)
    ui32Period = (SysCtlClockGet()) / 4;
    // Establecer el periodo del temporizador
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

    // Se habilita la interrupción por el TIMER0A
    IntEnable(INT_TIMER0A );
    // Se establece que exista la interrupción por Timeout
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Se habilitan las interrupciones Globales
    IntMasterEnable();
    // Se habilita el Timer
    TimerEnable(TIMER0_BASE, TIMER_A);

    //**********************************************************************************************************
    // Loop Principal
    //**********************************************************************************************************
   while (1)
    {


    }
}

















//MENSAJE DE PRUEBA UART

void uart_test(void)
{
    // Se manda mensajes por UART
    UARTCharPut(UART0_BASE, 'H');
    UARTCharPut(UART0_BASE, 'o');
    UARTCharPut(UART0_BASE, 'l');
    UARTCharPut(UART0_BASE, 'a');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'M');
    UARTCharPut(UART0_BASE, 'u');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 'd');
    UARTCharPut(UART0_BASE, 'o');
    UARTCharPut(UART0_BASE, 10);
    UARTCharPut(UART0_BASE, 13);
}


//**************************************************************************************************************
// Inicialización de UART
//**************************************************************************************************************
void InitUART(void)
{
    /*Enable the GPIO Port A*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*Enable the peripheral UART Module 0*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Make the UART pins be peripheral controlled. */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Sets the configuration of a UART. */
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    //Borramos todas las banderas principales antes de inicializar la interrupción
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX | UART_INT_FE | UART_INT_PE | UART_INT_BE | UART_INT_OE | UART_INT_RI | UART_INT_CTS | UART_INT_DCD | UART_INT_DSR);
    //Inicializamos la interrupción del uart0
    IntEnable(INT_UART0);
    //Inicializamos las interrupciones del módulo UART
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

}

//**************************************************************************************************************
// Handler de la interrupción del TIMER 0 - Recordar modificar el archivo tm4c123ght6pm_startup_css.c
//**************************************************************************************************************
void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Read the current state of the GPIO pin and
    // write back the opposite state
    Toggle();
}

//**************************************************************************************************************
// Handler de la interrupción del UART0 - Recordar modificar el archivo tm4c123ght6pm_startup_css.c
//**************************************************************************************************************
void UARTIntHandler(void){
//acá va la rutina que hace toggle a los leds de la tiva según el caracter que reciba
    //Clear the UART interrupt
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    //hacer una serie de ifs que detecten el caracter que se recibió
    receive = UARTCharGet(UART0_BASE);
    UARTCharPut(UART0_BASE, receive); //enviamos el caracter como prueba

}










//Función que cambia el estado de los pines
void Toggle(void)
{

    if (receive==114){ //114 decimal es r
        //Se recibió el comando para cambiar el led rojo
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
        } else {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }

    }
    if (receive==98){ //98 decimal es b
        //Se recibió el comando para cambiar el led azul
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
        } else {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }

    }
    if (receive ==103){ //103 en decimal es g
        //Se recibió el comando para cambiar el led verde
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
        } else {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }

    }
}
