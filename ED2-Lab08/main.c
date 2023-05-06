/* Universidad del Valle de Guatemala
 * Electrónica Digital 2
 * Laboratorio 8 - TivaWare
 * Autor: Luis Pablo Carranza
 * Fecha de creación: 5/5/23
 * Última modificación: 5/5/23
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "driverlib/pin_map.h"

/**
 * main.c
 */
#define SEMTIME 20000000
int PUSHREAD = 1;
char Dato;
char UltDat;
void ToggleLED(void);
void initUART(void);

int main(void)
{
    // Configuración del oscilador
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // Habilitar el puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Habilitar pines de salida
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    // Habilitar pin de entrada
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Habilitar interrupciones Globales
    IntMasterEnable();

    // Iniciar el UART
    initUART();

    // Iniciar el TIMER0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet()/2) - 1);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1){
        // No hace nada en el loop principal

        // Código del laboratorio
        //      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
        //      Hace lectura del PUSH1
        //      PUSHREAD = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
        //      Si leyó que presionaron entra a la condición
        //        if(PUSHREAD == 0){
        //            // Mientras sigan presionando el push sigue leyendo el valor hasta que deje de ser presionado
        //            while(PUSHREAD == 0){
        //                SysCtlDelay(SEMTIME/500);
        //                PUSHREAD = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
        //            }
        //            // Si dejó de ser presionado entra al semáforo
        //
        //            // Apago rojo y enciendo verde
        //            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
        //            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3);
        //            SysCtlDelay(SEMTIME);
        //
        //            int i;
        //
        //            for(i = 0; i < 5; i++){
        //                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
        //                SysCtlDelay(SEMTIME/10);
        //                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        //                SysCtlDelay(SEMTIME/10);
        //            }
        //
        //            // Enciendo verde para crear amarillo
        //            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_3|GPIO_PIN_1);
        //            SysCtlDelay(SEMTIME);
        //
        //            // Apago verde para que quede el rojo
        //            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
        //            SysCtlDelay(SEMTIME);
        //
        //
        //        }
    }
}

void Timer0IntHandler(void) {
    // Borrar la bandera de interrupción
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Llama a la función para el toggle del LED
    ToggleLED();

}

void UART0IntHandler(void) {
    // Borrar la bandera de interrupción
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    // Recibe el dato
    while(UARTCharsAvail(UART0_BASE)) {
        Dato = UARTCharGetNonBlocking(UART0_BASE);
        // Revisa si el dato recibido es igual al anterior
        UARTCharPutNonBlocking(UART0_BASE, Dato);
        UARTCharPutNonBlocking(UART0_BASE, '\n');
        if(Dato != UltDat){
            UltDat = Dato;  // Si no es igual es porque quiere hacer  un toggle
        }
        else{
            Dato = 'x';     // Si es igual quiere apagar el toggle
        }
    }
}

void initUART(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void ToggleLED(void){
    if(Dato == 'r'){
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }
    }
    else if(Dato == 'g'){
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)){
            GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }
    }
    else if(Dato == 'b'){
        if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)){
            GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }
    }
    else{
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
}
