/*
 * math calculator i2c slave stm32
 * Copyright (C) 2017  Amitesh Singh <singh.amitesh@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

extern "C"
{
   #include <libopencm3/cm3/common.h>
   #include <libopencm3/stm32/rcc.h>
   #include <libopencm3/stm32/gpio.h>
   #include <libopencm3/stm32/i2c.h>
   #include <libopencm3/cm3/nvic.h>
}

static void
my_delay_1( void )
{
   int i = 72e6/2/4;

   while( i > 0 )
     {
        i--;
        __asm__( "nop" );
     }
}

#define MYSLAVE_ADDRESS 0x32
//Set Commands
#define MYSLAVE_SET_REG 0x01
//GET commands
#define MYSLAVE_GET_ADD_RESULT 0x02
#define MYSLAVE_GET_SUB_RESULT 0x03
#define MYSLAVE_GET_MUL_RESULT 0x04

static void
i2c_slave_init(uint8_t ownaddress)
{
   rcc_periph_clock_enable(RCC_GPIOB);
   rcc_periph_clock_enable(RCC_I2C1);

   nvic_enable_irq(NVIC_I2C1_EV_IRQ);

   // configure i2c pins
   gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                 GPIO_I2C1_SDA); //PB7
   gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                 GPIO_I2C1_SCL); //PB6

   i2c_reset(I2C1);
   i2c_peripheral_disable(I2C1);

   i2c_set_speed(I2C1, i2c_speed_sm_100k, I2C_CR2_FREQ_36MHZ);
   i2c_set_own_7bit_slave_address(I2C1, ownaddress);
   i2c_enable_interrupt(I2C1, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
   i2c_peripheral_enable(I2C1);

   // slave needs to acknowledge on receiving bytes
   // set it after enabling Peripheral i.e. PE = 1
   i2c_enable_ack(I2C1);
}

volatile uint8_t *read_p;
volatile uint8_t *write_p;
volatile uint8_t writing;
volatile uint8_t reading;

volatile uint8_t buf[3];
volatile uint16_t val;

//i2c1 event ISR
extern "C" void i2c1_ev_isr(void)
{
   uint32_t sr1, sr2;

   sr1 = I2C_SR1(I2C1);

   if (sr1 & I2C_SR1_ADDR)
     {
        reading = 0;
        read_p = buf;
        write_p = ((volatile uint8_t *)(&val) + 1);
        writing = 2;
        //Clear the ADDR sequence by reading SR2.
        sr2 = I2C_SR2(I2C1);
        (void) sr2;
     }
   else if (sr1 & I2C_SR1_RxNE)
     {
        //ignore more than 3 bytes reading
        if (reading > 3)
          return;
        //read bytes from slave
        *read_p++ = i2c_get_data(I2C1);
        reading++;
     }
   else if ((sr1 & I2C_SR1_TxE) && !(sr1 & I2C_SR1_BTF))
     {
        //send data to master in MSB order
        i2c_send_data(I2C1, *write_p--);
        writing--;
     }
   // done by master by sending STOP
   //this event happens when slave is in Recv mode at the end of communication
   else if (sr1 & I2C_SR1_STOPF)
     {
        i2c_peripheral_enable(I2C1);

        if (buf[0] == MYSLAVE_GET_ADD_RESULT)
          val = buf[1] + buf[2];
        else if (buf[0] == MYSLAVE_GET_SUB_RESULT)
          val = buf[1] - buf[2];
        else if (buf[0] == MYSLAVE_GET_MUL_RESULT)
          val = buf[1] * buf[2];
     }
   //this event happens when slave is in transmit mode at the end of communication
   else if (sr1 & I2C_SR1_AF)
     {
        //(void) I2C_SR1(I2C1);
        I2C_SR1(I2C1) &= ~(I2C_SR1_AF);
     }
}

int main( void )
{
   //set STM32 to 72 MHz
   rcc_clock_setup_in_hse_8mhz_out_72mhz();

   // Enable GPIOC clock
   rcc_periph_clock_enable(RCC_GPIOC);
   //Set GPIO13 (inbuild led connected) to 'output push-pull'
   gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                 GPIO13);
   //switch led off
   gpio_set(GPIOC, GPIO13);

   //initialize i2c slave
   i2c_slave_init(MYSLAVE_ADDRESS);

   while( 1 )
     {
        gpio_toggle(GPIOC, GPIO13);
        my_delay_1();
     }
}
